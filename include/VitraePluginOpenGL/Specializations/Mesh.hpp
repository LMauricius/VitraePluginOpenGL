#pragma once

#include "Vitrae/Assets/Shapes/Mesh.hpp"
#include "Vitrae/Containers/StableMap.hpp"
#include "Vitrae/Data/Typedefs.hpp"

#include "glad/glad.h"

#include <map>
#include <optional>

namespace Vitrae
{
class OpenGLRenderer;

class OpenGLMesh : public Mesh
{
  public:
    OpenGLMesh(const AssimpLoadParams &params);
    OpenGLMesh(const TriangleVerticesParams &params);
    ~OpenGLMesh();

    void prepareComponents(const ParamList &components) override;
    void loadToGPU(Renderer &rend) override;
    void unloadFromGPU();

    FrontSideOrientation getFrontSideOrientation() const override;

    BoundingBox getBoundingBox() const override;

    inline std::size_t memory_cost() const override
    {
        /// TODO: compute the real cost
        return sizeof(*this);
    }

    StructSubImageVariantPtr getVertexComponentImage(StringId componentName) const override;
    void setVertexComponentImage(StringId componentName,
                                 StructSubImageVariantPtr p_buffer) override;

    StructImagePtr<void, Triangle> getIndexImage() const override;

    void rasterize() const override;

    GLuint VAO;

  protected:
    ComponentRoot &m_root;
    String m_friendlyname;
    FrontSideOrientation m_frontSideOrientation;
    BoundingBox m_aabb;
    StableMap<StringId, StructSubImageVariantPtr> m_vertexComponentImages;
    StructImagePtr<void, Triangle> m_indexImage;

    bool m_sentToGPU;
};
} // namespace Vitrae