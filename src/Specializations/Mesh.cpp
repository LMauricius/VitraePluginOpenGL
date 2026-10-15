#include "VitraePluginOpenGL/Specializations/Mesh.hpp"
#include "Vitrae/Assets/Material.hpp"
#include "Vitrae/Collections/AssimpConv.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Collections/MeshGenerator.hpp"
#include "Vitrae/Data/Typedefs.hpp"
#include "Vitrae/TypeConversion/AssimpCvt.hpp"
#include "Vitrae/TypeConversion/StringCvt.hpp"
#include "VitraePluginOpenGL/Specializations/BinaryImage.hpp"
#include "VitraePluginOpenGL/Specializations/Renderer.hpp"

#include <map>
#include <vector>

namespace Vitrae
{
OpenGLMesh::OpenGLMesh(const AssimpLoadParams &params)
    : m_root(params.root), m_frontSideOrientation{FrontSideOrientation::CounterClockwise},
      m_aabb{{}, {}},
      m_indexImage{
          makeStructImage<void, Triangle>(params.root,
                                          ImageUsageHint::HOST_INIT | ImageUsageHint::GPU_DRAW,
                                          params.p_extMesh->mNumFaces),
      },
      m_sentToGPU(false)
{
    OpenGLRenderer &rend = static_cast<OpenGLRenderer &>(params.root.getComponent<Renderer>());
    AssimpConvCollection &conv = params.root.getComponent<AssimpConvCollection>();

    // load triangles
    auto cpuTriangles = m_indexImage.getMutableElements();
    if (params.p_extMesh->HasFaces()) {
        for (int i = 0; i < params.p_extMesh->mNumFaces; i++) {
            if (params.p_extMesh->mFaces[i].mNumIndices == 3) {
                for (int j = 0; j < params.p_extMesh->mFaces[i].mNumIndices; j++) {
                    cpuTriangles[i].ind[j] = params.p_extMesh->mFaces[i].mIndices[j];
                }
            } else {
                cpuTriangles[i].ind[0] = 0;
                cpuTriangles[i].ind[1] = 0;
                cpuTriangles[i].ind[2] = 0;
            }
        }
    }

    m_indexImage.getBinaryImage()->synchronize();

    // load vertices
    auto extractVertexData =
        [&]<class aiType, class glmType = typename aiTypeCvt<aiType>::glmType>(
            std::span<const AssimpConvCollection::MeshBufferConv<aiType>> aiBufferInfos) {
            for (auto &info : aiBufferInfos) {
                // get buffers
                std::size_t layoutInd = rend.getVertexLayoutIndex(info.name);
                const aiType *src = info.extractor(*params.p_extMesh);

                // fill buffers
                if (src != nullptr) {
                    // construct buffer
                    StructImagePtr<void, glmType> p_verImg = makeStructImage<void, glmType>(
                        params.root, ImageUsageHint::HOST_INIT | ImageUsageHint::GPU_DRAW,
                        params.p_extMesh->mNumVertices);
                    m_vertexComponentImages.emplace(info.name, p_verImg);

                    std::span<glmType> cpuVerImg = p_verImg.getMutableElements();

                    for (int i = 0; i < params.p_extMesh->mNumVertices; i++) {
                        cpuVerImg[i] = aiTypeCvt<aiType>::toGlmVal(src[i]);
                    }

                    p_verImg.getBinaryImage()->synchronize();
                }
            }
        };

    extractVertexData(conv.getMeshBufferConvs<aiVector2D>());
    extractVertexData(conv.getMeshBufferConvs<aiVector3D>());
    extractVertexData(conv.getMeshBufferConvs<aiColor3D>());
    extractVertexData(conv.getMeshBufferConvs<aiColor4D>());

    m_aabb = {
        {
            params.p_extMesh->mAABB.mMin.x,
            params.p_extMesh->mAABB.mMin.y,
            params.p_extMesh->mAABB.mMin.z,
        },
        {
            params.p_extMesh->mAABB.mMax.x,
            params.p_extMesh->mAABB.mMax.y,
            params.p_extMesh->mAABB.mMax.z,
        },
    };

    // debug
    m_friendlyname = toString(params.p_extMesh->mName);

    // sanity check
    for (auto &tri : cpuTriangles) {
        if (tri.ind[0] > params.p_extMesh->mNumVertices ||
            tri.ind[1] > params.p_extMesh->mNumVertices ||
            tri.ind[2] > params.p_extMesh->mNumVertices) {
            throw std::runtime_error("invalid triangle index in mesh " + m_friendlyname);
        }
    }
}

OpenGLMesh::OpenGLMesh(const TriangleVerticesParams &params)
    : m_root(params.root), m_friendlyname(params.friendlyname),
      m_frontSideOrientation(FrontSideOrientation::CounterClockwise), m_aabb{{}, {}},
      m_vertexComponentImages(params.vertexComponentImages), m_indexImage(params.indexImage),
      m_sentToGPU(false)
{}

OpenGLMesh::~OpenGLMesh()
{
    unloadFromGPU();
}

void OpenGLMesh::prepareComponents(const ParamList &components)
{
    MeshGeneratorCollection &meshGenerators = m_root.getComponent<MeshGeneratorCollection>();

    for (auto componentname : components.getSpecNameIds()) {
        if (m_vertexComponentImages.find(componentname) == m_vertexComponentImages.end()) {
            MeshGenerator generator = meshGenerators.getGeneratorForComponent(componentname);
            if (generator) {
                unloadFromGPU();

                auto newComponentImages = generator(m_root, *this);

                for (auto [name, p_verImg] : newComponentImages) {
                    m_vertexComponentImages.insert_or_assign(name, p_verImg);
                }
            }
        }
    }
}

void OpenGLMesh::loadToGPU(Renderer &r)
{
    OpenGLRenderer &rend = static_cast<OpenGLRenderer &>(r);

    if (!m_sentToGPU) {
        // prepare OpenGL buffers
        glGenVertexArrays(1, &VAO);

        // load vertices
        glBindVertexArray(VAO);

        for (auto [name, p_verImg] : m_vertexComponentImages) {
            OpenGLBinaryImage &binVerImg =
                static_cast<OpenGLBinaryImage &>(*(p_verImg.getBinaryImage()));

            if (!binVerImg.isSynchronized()) {
                throw std::runtime_error("Mesh vertex component image for " +
                                         String(m_friendlyname) + " is not synchronized");
            }

            // get type info and conversion
            const TypeInfo &compType = p_verImg.getHeaderTypeInfo();
            const VectorMeta *const p_vectorMeta =
                dynamic_cast<const VectorMeta *>(&compType.metaDetail);
            const TypeInfo &subCompType = p_vectorMeta ? p_vectorMeta->componentTypeInfo : compType;
            std::size_t numSubComponents = p_vectorMeta ? p_vectorMeta->numComponents : 1;

            const GLConversionSpec &convSpec = rend.getTypeConversion(subCompType);

            // assert that the type is convertible to a vertex array
            // OpenGL supports up to 4 components per vertex attribute
            /// TODO: Allow bigger types by separating them into different locations
            if (numSubComponents == 0 || numSubComponents > 4 ||
                convSpec.glTypeSpec.layout.indexSize != 1) {
                throw std::runtime_error(
                    "Unsupported component type " + String(compType.getShortTypeName()) +
                    " for mesh " + m_friendlyname +
                    "  due to invalid number of components or locations taken.");
            }
            if (!convSpec.scalarSpec.has_value()) {
                throw std::runtime_error(
                    "Unsupported component type " + String(compType.getShortTypeName()) +
                    " for mesh " + m_friendlyname +
                    "  due its component value type not being primitive according to conversion.");
            }
            const GLScalarSpec &scalarSpec = convSpec.scalarSpec.value();

            // send to OpenGL
            std::size_t layoutInd = rend.getVertexLayoutIndex(name);
            glBindBuffer(GL_ARRAY_BUFFER, binVerImg.getGlBufferHandle());
            glVertexAttribPointer(layoutInd,                        // layout pos
                                  numSubComponents,                 // data structure info
                                  scalarSpec.glTypeId,              //
                                  scalarSpec.isNormalized,          //
                                  p_verImg.getBytesStride(),        // data subbuffer location info
                                  (void *)p_verImg.getBytesOffset() //
            );
            glEnableVertexAttribArray(layoutInd);
        }

        OpenGLBinaryImage &binIndImg =
            static_cast<OpenGLBinaryImage &>(*(m_indexImage.getBinaryImage()));

        if (!binIndImg.isSynchronized()) {
            throw std::runtime_error("Mesh index buffer for " + String(m_friendlyname) +
                                     " is not synchronized");
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, binIndImg.getGlBufferHandle());

        glBindVertexArray(0);

        // debug
        String glLabel = String("mesh ") + String(m_friendlyname);
        glObjectLabel(GL_VERTEX_ARRAY, VAO, glLabel.size(), glLabel.data());

        m_sentToGPU = true;
    }
}

void OpenGLMesh::unloadFromGPU()
{
    if (m_sentToGPU) {
        m_sentToGPU = false;
        glDeleteVertexArrays(1, &VAO);
    }
}

FrontSideOrientation OpenGLMesh::getFrontSideOrientation() const
{
    return m_frontSideOrientation;
}

BoundingBox OpenGLMesh::getBoundingBox() const
{
    return m_aabb;
}

StructSubImageVariantPtr OpenGLMesh::getVertexComponentImage(StringId componentName) const
{
    return m_vertexComponentImages.at(componentName);
}

void OpenGLMesh::setVertexComponentImage(StringId componentName, StructSubImageVariantPtr p_buffer)
{
    unloadFromGPU();
    m_vertexComponentImages[componentName] = p_buffer;
}

StructImagePtr<void, Triangle> OpenGLMesh::getIndexImage() const
{
    return m_indexImage;
}

void OpenGLMesh::rasterize() const
{
    if (m_sentToGPU) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3 * m_indexImage.numElements(), GL_UNSIGNED_INT, 0);
    }
}

} // namespace Vitrae