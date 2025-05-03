#include "VitraePluginOpenGL/Specializations/Compositing/IndexRender.hpp"
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

#include "Vitrae/Params/Standard.hpp"

#include "MMeter.h"

namespace Vitrae
{

// OpenGLComposeIndexRender::OpenGLComposeIndexRender(const SetupParams &params) {}

OpenGLComposeIndexRender::OpenGLComposeIndexRender(const SetupParams &params): m_params(params) {
    for (const auto &spec : params.inputTokenNames) {
        m_inputSpecs.insert_back({.name = spec, .typeInfo = TYPE_INFO<void>});
    }
    for (const auto &spec : params.outputTokenNames) {
        m_outputSpecs.insert_back({.name = spec, .typeInfo = TYPE_INFO<void>});
    }

    m_inputSpecs.insert_back({.name = params.sizeParamName, .typeInfo = TYPE_INFO<std::uint32_t>});
}

std::size_t OpenGLComposeIndexRender::memory_cost() const
{
    return sizeof(*this);
}


const ParamList &OpenGLComposeIndexRender::getInputSpecs(const ParamAliases &aliases) const
{
    if (auto it = m_specsPerKey.find(getSpecsKey(aliases)); it != m_specsPerKey.end()) {
        return (*it).second->inputSpecs;
    } else {
        return m_inputSpecs;
    }
}

const ParamList &OpenGLComposeIndexRender::getOutputSpecs() const
{
    return m_outputSpecs;
}

const ParamList &OpenGLComposeIndexRender::getFilterSpecs(const ParamAliases &aliases) const
{
    if (auto it = m_specsPerKey.find(getSpecsKey(aliases)); it != m_specsPerKey.end()) {
        return (*it).second->filterSpecs;
    } else {
        return m_filterSpecs;
    }
}

const ParamList &OpenGLComposeIndexRender::getConsumingSpecs(const ParamAliases &aliases) const
{
    if (auto it = m_specsPerKey.find(getSpecsKey(aliases)); it != m_specsPerKey.end()) {
        return (*it).second->consumingSpecs;
    } else {
        return m_consumingSpecs;
    }
}

void OpenGLComposeIndexRender::extractUsedTypes(std::set<const TypeInfo *> &typeSet,
                                                const ParamAliases &aliases) const
{
    for (const ParamList *p_specs :
         {&m_inputSpecs, &m_filterSpecs, &m_consumingSpecs, &m_outputSpecs}) {
        for (const ParamSpec &spec : p_specs->getSpecList()) {
            typeSet.insert(&spec.typeInfo);
        }
    }
}

void OpenGLComposeIndexRender::extractSubTasks(std::set<const Task *> &taskSet,
                                               const ParamAliases &aliases) const
{
    taskSet.insert(this);
}

void OpenGLComposeIndexRender::run(RenderComposeContext ctx) const
{
    MMETER_SCOPE_PROFILER(m_friendlyName.c_str());

    OpenGLRenderer &rend = static_cast<OpenGLRenderer &>(m_params.root.getComponent<Renderer>());
    CompiledGLSLShaderCacher &shaderCacher = m_params.root.getComponent<CompiledGLSLShaderCacher>();

    // Get specs cache and init it if needed
    bool needsRebuild = false;
    std::size_t specsKey = getSpecsKey(ctx.aliases);
    auto specsIt = m_specsPerKey.find(specsKey);
    if (specsIt == m_specsPerKey.end()) {
        specsIt = m_specsPerKey.emplace(specsKey, new SpecsPerAliases()).first;
        SpecsPerAliases &specsContainer = *(*specsIt).second;

        specsContainer.inputSpecs.merge(m_inputSpecs);
        specsContainer.filterSpecs.merge(m_filterSpecs);
        specsContainer.consumingSpecs.merge(m_consumingSpecs);

        needsRebuild = true;
    }

    SpecsPerAliases &specsContainer = *(*specsIt).second;

    // extract common inputs
    dynasma::FirmPtr<FrameStore> p_frame =
        ctx.properties.get(StandardParam::fs_target.name).get<dynasma::FirmPtr<FrameStore>>();
    OpenGLFrameStore &frame = static_cast<OpenGLFrameStore &>(*p_frame);
    std::uint32_t indexSize = ctx.properties.get(m_params.sizeParamName).get<std::uint32_t>();

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

    const ParamAliases *p_aliaseses[] = {&p_mat->getParamAliases(), &ctx.aliases};
    ParamAliases combinedAliases(p_aliaseses);

    // Compile and setup the shader
    dynasma::FirmPtr<CompiledGLSLShader> p_compiledShader;
    GLint gl_index4data_UniformLocation;

    {
        MMETER_SCOPE_PROFILER("Shader setup");

        // compile shader for this material
        {
            MMETER_SCOPE_PROFILER("Shader loading");

            p_compiledShader = shaderCacher.retrieve_asset({CompiledGLSLShader::SurfaceShaderParams(
                combinedAliases, m_params.rasterizing.vertexPositionOutputPropertyName,
                *frame.getRenderComponents(), m_params.root)});

            // Aliases should've already been taken into account, so use properties directly
            VariantScope &directProperties = ctx.properties.getUnaliasedScope();

            // Store pipeline property specs
            needsRebuild |= (specsContainer.inputSpecs.merge(p_compiledShader->inputSpecs) > 0);
            needsRebuild |= (specsContainer.filterSpecs.merge(p_compiledShader->filterSpecs) > 0);
            needsRebuild |=
                (specsContainer.consumingSpecs.merge(p_compiledShader->consumingSpecs) > 0);

            if (needsRebuild) {
                throw ComposeTaskRequirementsChangedException();
            }

            if (auto it = p_compiledShader->uniformSpecs.find(StandardParam::index4data.name);
                it != p_compiledShader->uniformSpecs.end()) {
                    gl_index4data_UniformLocation = (*it).second.location;
            } else {
                gl_index4data_UniformLocation = -1;
            }

            glUseProgram(p_compiledShader->programGLName);
        }

        // Aliases should've already been taken into account, so use properties directly
        VariantScope &directProperties = ctx.properties.getUnaliasedScope();

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

            for (std::uint32_t i = 0; i < indexSize; ++i) {
                if (gl_index4data_UniformLocation != -1) {
                    glUniform1i(gl_index4data_UniformLocation, i);
                }

                rasterizeShape(*p_shape, m_params.rasterizing);
            }

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

void OpenGLComposeIndexRender::prepareRequiredLocalAssets(RenderComposeContext ctx) const {}

StringView OpenGLComposeIndexRender::getFriendlyName() const
{
    return m_friendlyName;
}

std::size_t OpenGLComposeIndexRender::getSpecsKey(const ParamAliases &aliases) const
{
    return combinedHashes<2>(
        {{std::hash<StringId>{}(m_params.rasterizing.vertexPositionOutputPropertyName),
          aliases.hash()}});
}

} // namespace Vitrae