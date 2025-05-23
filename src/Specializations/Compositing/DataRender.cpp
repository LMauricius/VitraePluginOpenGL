#include "VitraePluginOpenGL/Specializations/Compositing/DataRender.hpp"
#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/Assets/Material.hpp"
#include "Vitrae/Assets/Model.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Dynamic/VariantScope.hpp"
#include "Vitrae/Params/Standard.hpp"
#include "VitraePluginOpenGL/Bits/RenderBits.hpp"
#include "VitraePluginOpenGL/Specializations/FrameStore.hpp"
#include "VitraePluginOpenGL/Specializations/Renderer.hpp"
#include "VitraePluginOpenGL/Specializations/ShaderCompilation.hpp"
#include "VitraePluginOpenGL/Specializations/Texture.hpp"

#include "MMeter.h"

namespace Vitrae
{

OpenGLComposeDataRender::OpenGLComposeDataRender(const SetupParams &params)
    : m_root(params.root), m_params(params)
{
    m_friendlyName = "Render data:";
    for (const auto &spec : params.outputSpecs.getSpecList()) {
        m_friendlyName += "\n- " + spec.name;
    }

    m_params.inputSpecs.insert_back(StandardParam::mat_view);
    m_params.inputSpecs.insert_back(StandardParam::mat_proj);
    m_params.filterSpecs.insert_back(StandardParam::fs_target);
}

std::size_t OpenGLComposeDataRender::memory_cost() const
{
    /// TODO: compute the real cost
    return sizeof(OpenGLComposeDataRender);
}

const ParamList &OpenGLComposeDataRender::getInputSpecs(const ParamAliases &aliases) const
{
    if (auto it = m_specsPerKey.find(getSpecsKey(aliases)); it != m_specsPerKey.end()) {
        return (*it).second->inputSpecs;
    } else {
        return m_params.inputSpecs;
    }
}

const ParamList &OpenGLComposeDataRender::getOutputSpecs() const
{
    return m_params.outputSpecs;
}

const ParamList &OpenGLComposeDataRender::getFilterSpecs(const ParamAliases &aliases) const
{
    if (auto it = m_specsPerKey.find(getSpecsKey(aliases)); it != m_specsPerKey.end()) {
        return (*it).second->filterSpecs;
    } else {
        return m_params.filterSpecs;
    }
}

const ParamList &OpenGLComposeDataRender::getConsumingSpecs(const ParamAliases &aliases) const
{
    if (auto it = m_specsPerKey.find(getSpecsKey(aliases)); it != m_specsPerKey.end()) {
        return (*it).second->consumingSpecs;
    } else {
        return m_params.consumingSpecs;
    }
}

void OpenGLComposeDataRender::extractUsedTypes(std::set<const TypeInfo *> &typeSet,
                                               const ParamAliases &aliases) const
{
    if (auto it = m_specsPerKey.find(getSpecsKey(aliases)); it != m_specsPerKey.end()) {
        const SpecsPerAliases &specsPerAliases = *(*it).second;

        for (const ParamList *p_specs :
             {&specsPerAliases.inputSpecs, &m_params.outputSpecs, &specsPerAliases.filterSpecs,
              &specsPerAliases.consumingSpecs}) {
            for (const ParamSpec &spec : p_specs->getSpecList()) {
                typeSet.insert(&spec.typeInfo);
            }
        }
    }
}

void OpenGLComposeDataRender::extractSubTasks(std::set<const Task *> &taskSet,
                                              const ParamAliases &aliases) const
{
    taskSet.insert(this);
}

void OpenGLComposeDataRender::run(RenderComposeContext args) const
{
    MMETER_SCOPE_PROFILER(m_friendlyName.c_str());

    OpenGLRenderer &rend = static_cast<OpenGLRenderer &>(m_root.getComponent<Renderer>());
    CompiledGLSLShaderCacher &shaderCacher = m_root.getComponent<CompiledGLSLShaderCacher>();

    // Get specs cache and init it if needed
    bool needsRebuild = false;
    std::size_t specsKey = getSpecsKey(args.aliases);
    auto specsIt = m_specsPerKey.find(specsKey);
    if (specsIt == m_specsPerKey.end()) {
        specsIt = m_specsPerKey.emplace(specsKey, new SpecsPerAliases()).first;
        SpecsPerAliases &specsContainer = *(*specsIt).second;

        specsContainer.inputSpecs.merge(m_params.inputSpecs);
        specsContainer.filterSpecs.merge(m_params.filterSpecs);
        specsContainer.consumingSpecs.merge(m_params.consumingSpecs);

        needsRebuild = true;
    }

    SpecsPerAliases &specsContainer = *(*specsIt).second;

    // extract common inputs
    dynasma::FirmPtr<FrameStore> p_frame =
        args.properties.get(StandardParam::fs_target.name).get<dynasma::FirmPtr<FrameStore>>();
    OpenGLFrameStore &frame = static_cast<OpenGLFrameStore &>(*p_frame);
    glm::mat4 mat_view =
        args.properties.get(args.aliases.choiceFor(StandardParam::mat_view.name)).get<glm::mat4>();
    glm::mat4 mat_proj =
        args.properties.get(args.aliases.choiceFor(StandardParam::mat_proj.name)).get<glm::mat4>();
    glm::mat4 mat_display = mat_proj * mat_view;

    LoDSelectionParams lodParams = {
        .method = LoDSelectionMethod::Maximum,
        .threshold =
            {
                .minElementSize = 1.0f,
            },
    };
    LoDContext lodCtx = {
        .closestPointScaling = 1.0f,
    };

    auto p_model = m_params.p_dataPointModel.getLoaded();
    auto p_shape =
        p_model->getBestForm(m_params.rasterizing.modelFormPurpose, lodParams, lodCtx).getLoaded();

    auto p_mat = p_model->getMaterial().getLoaded();

    const ParamAliases *p_aliaseses[] = {&p_mat->getParamAliases(), &args.aliases};
    ParamAliases combinedAliases(p_aliaseses);

    // Compile and setup the shader
    dynasma::FirmPtr<CompiledGLSLShader> p_compiledShader;
    GLint glModelMatrixUniformLocation;
    GLint glMVPMatrixUniformLocation;
    GLint glDisplayMatrixUniformLocation;

    {
        MMETER_SCOPE_PROFILER("Shader setup");

        // compile shader for this material
        {
            MMETER_SCOPE_PROFILER("Shader loading");

            p_compiledShader = shaderCacher.retrieve_asset({CompiledGLSLShader::SurfaceShaderParams(
                combinedAliases, m_params.rasterizing.vertexPositionOutputPropertyName,
                *frame.getRenderComponents(), m_root)});

            // Aliases should've already been taken into account, so use properties directly
            VariantScope &directProperties = args.properties.getUnaliasedScope();

            // Store pipeline property specs
            needsRebuild |= (specsContainer.inputSpecs.merge(p_compiledShader->inputSpecs) > 0);
            needsRebuild |= (specsContainer.filterSpecs.merge(p_compiledShader->filterSpecs) > 0);
            needsRebuild |=
                (specsContainer.consumingSpecs.merge(p_compiledShader->consumingSpecs) > 0);

            if (needsRebuild) {
                throw ComposeTaskRequirementsChangedException();
            }

            if (auto it = p_compiledShader->uniformSpecs.find(StandardParam::mat_model.name);
                it != p_compiledShader->uniformSpecs.end()) {
                glModelMatrixUniformLocation = (*it).second.location;
            } else {
                glModelMatrixUniformLocation = -1;
            }
            if (auto it = p_compiledShader->uniformSpecs.find(StandardParam::mat_display.name);
                it != p_compiledShader->uniformSpecs.end()) {
                glDisplayMatrixUniformLocation = (*it).second.location;
            } else {
                glDisplayMatrixUniformLocation = -1;
            }
            if (auto it = p_compiledShader->uniformSpecs.find(StandardParam::mat_mvp.name);
                it != p_compiledShader->uniformSpecs.end()) {
                glMVPMatrixUniformLocation = (*it).second.location;
            } else {
                glMVPMatrixUniformLocation = -1;
            }

            glUseProgram(p_compiledShader->programGLName);
        }

        // Aliases should've already been taken into account, so use properties directly
        VariantScope &directProperties = args.properties.getUnaliasedScope();

        {
            MMETER_SCOPE_PROFILER("Uniform setup");

            p_compiledShader->setupProperties(rend, directProperties, *p_mat);
        }
    }
    {
        MMETER_SCOPE_PROFILER("Shape loading");

        p_shape->prepareComponents(p_compiledShader->vertexComponentSpecs);
        p_shape->loadToGPU(rend);
    }

    // render
    {
        MMETER_SCOPE_PROFILER("Rendering");

        switch (m_params.rasterizing.rasterizingMode) {
        // derivational methods (all methods for now)
        case RasterizingMode::DerivationalFillCenters:
        case RasterizingMode::DerivationalFillEdges:
        case RasterizingMode::DerivationalFillVertices:
        case RasterizingMode::DerivationalTraceEdges:
        case RasterizingMode::DerivationalTraceVertices:
        case RasterizingMode::DerivationalDotVertices: {
            frame.enterRender({0.0f, 0.0f}, {1.0f, 1.0f});

            stateSetupRasterizing(m_params.rasterizing);

            // run the data generator

            RenderCallback renderCallback = [glModelMatrixUniformLocation,
                                             glDisplayMatrixUniformLocation,
                                             glMVPMatrixUniformLocation, &mat_display, p_shape,
                                             &rend,
                                             &m_params = m_params](const glm::mat4 &transform) {
                if (glModelMatrixUniformLocation != -1) {
                    glUniformMatrix4fv(glModelMatrixUniformLocation, 1, GL_FALSE,
                                       &(transform[0][0]));
                }
                if (glDisplayMatrixUniformLocation != -1) {
                    glUniformMatrix4fv(glDisplayMatrixUniformLocation, 1, GL_FALSE,
                                       &(mat_display[0][0]));
                }
                if (glMVPMatrixUniformLocation != -1) {
                    glm::mat4 mat_mvp = mat_display * transform;
                    glUniformMatrix4fv(glMVPMatrixUniformLocation, 1, GL_FALSE, &(mat_mvp[0][0]));
                }

                rasterizeShape(*p_shape, m_params.rasterizing);
            };

            m_params.dataGenerator(args, renderCallback);

            glUseProgram(0);

            frame.exitRender();
            break;
        }
        }
    }

    // wait (for profiling)
#ifdef VITRAE_ENABLE_DETERMINISTIC_RENDERING
    {
        MMETER_SCOPE_PROFILER("Waiting for GL operations");

        glFinish();
    }
#endif
}

void OpenGLComposeDataRender::prepareRequiredLocalAssets(RenderComposeContext ctx) const {}

StringView OpenGLComposeDataRender::getFriendlyName() const
{
    return m_friendlyName;
}

std::size_t OpenGLComposeDataRender::getSpecsKey(const ParamAliases &aliases) const
{
    return combinedHashes<2>(
        {{std::hash<StringId>{}(m_params.rasterizing.vertexPositionOutputPropertyName),
          aliases.hash()}});
}

} // namespace Vitrae