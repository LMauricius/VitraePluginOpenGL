#include "VitraePluginOpenGL/Specializations/Compositing/ClearRender.hpp"
#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Data/Overloaded.hpp"
#include "Vitrae/Dynamic/TypeInfo.hpp"
#include "Vitrae/Params/Standard.hpp"
#include "Vitrae/TypeConversion/StringCvt.hpp"
#include "VitraePluginOpenGL/Specializations/FrameStore.hpp"
#include "VitraePluginOpenGL/Specializations/Mesh.hpp"
#include "VitraePluginOpenGL/Specializations/Renderer.hpp"
#include "VitraePluginOpenGL/Specializations/ShaderCompilation.hpp"
#include "VitraePluginOpenGL/Specializations/Texture.hpp"

#include "MMeter.h"

namespace Vitrae
{

namespace
{
const ParamList FILTER_SPECS = {StandardParam::fs_target};
}

OpenGLComposeClearRender::OpenGLComposeClearRender(const SetupParams &params)
    : m_root(params.root), m_friendlyName(String("Clear target\n"))
{
    for (auto &tokenName : params.outputTokenNames) {
        m_outputSpecs.insert_back({.name = tokenName, .typeInfo = TYPE_INFO<void>});
    }
}

std::size_t OpenGLComposeClearRender::memory_cost() const
{
    /// TODO: calculate the real memory cost
    return sizeof(*this);
}

const ParamList &OpenGLComposeClearRender::getInputSpecs(const ParamAliases &) const
{
    return EMPTY_PROPERTY_LIST;
}

const ParamList &OpenGLComposeClearRender::getOutputSpecs() const
{
    return m_outputSpecs;
}

const ParamList &OpenGLComposeClearRender::getFilterSpecs(const ParamAliases &) const
{
    return FILTER_SPECS;
}

const ParamList &OpenGLComposeClearRender::getConsumingSpecs(const ParamAliases &) const
{
    return EMPTY_PROPERTY_LIST;
}

void OpenGLComposeClearRender::extractUsedTypes(std::set<const TypeInfo *> &typeSet,
                                                const ParamAliases &aliases) const
{
    for (const ParamList *p_specs : {&m_outputSpecs, &FILTER_SPECS}) {
        for (const ParamSpec &spec : p_specs->getSpecList()) {
            typeSet.insert(&spec.typeInfo);
        }
    }
}

void OpenGLComposeClearRender::extractSubTasks(std::set<const Task *> &taskSet,
                                               const ParamAliases &aliases) const
{
    taskSet.insert(this);
}

void OpenGLComposeClearRender::run(RenderComposeContext args) const
{
    MMETER_SCOPE_PROFILER(m_friendlyName.c_str());

    dynasma::FirmPtr<FrameStore> p_frame =
        args.properties.get(StandardParam::fs_target.name).get<dynasma::FirmPtr<FrameStore>>();
    OpenGLFrameStore &frame = static_cast<OpenGLFrameStore &>(*p_frame);

    frame.enterRender({0.0f, 0.0f}, {1.0f, 1.0f});

    glDepthMask(GL_TRUE);
    GLbitfield clearMask = 0;

    std::size_t attachmentIndex = 0;
    for (auto &texSpec : frame.getOutputTextureSpecs()) {

        // Either set the clear color for standard buffers, or clear buffer
        // manually
        std::visit(Overloaded{
                       [&](const FixedRenderComponent &comp) {
                           switch (comp) {
                           case FixedRenderComponent::Depth:
                               // Check the color type
                               std::visit(Overloaded{
                                              [&](const FixedClearColor &colInd) {
                                                  switch (colInd) {
                                                  case FixedClearColor::Garbage:
                                                      // do nothing
                                                      break;
                                                  case FixedClearColor::Default:
                                                      // default depth 1.0
                                                      glClearDepthf(1.0f);
                                                      clearMask |= GL_DEPTH_BUFFER_BIT;
                                                      break;
                                                  }
                                              },
                                              [&](const glm::vec4 &color) {
                                                  // first component is depth
                                                  glClearDepthf(color[0]);
                                                  clearMask |= GL_DEPTH_BUFFER_BIT;
                                              },
                                          },
                                          texSpec.clearColor);

                               break;
                           }
                       },
                       [&](const ParamSpec &spec) {
                           if (attachmentIndex == 0) {
                               // default color output
                               // Check the color type
                               std::visit(Overloaded{
                                              [&](const FixedClearColor &colInd) {
                                                  switch (colInd) {
                                                  case FixedClearColor::Garbage:
                                                      // do nothing
                                                      break;
                                                  case FixedClearColor::Default:
                                                      // default is black
                                                      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                                                      clearMask |= GL_COLOR_BUFFER_BIT;
                                                      break;
                                                  }
                                              },
                                              [&](const glm::vec4 &color) {
                                                  // use the vector
                                                  glClearColor(color[0], color[1], color[2], color[3]);
                                                  clearMask |= GL_COLOR_BUFFER_BIT;
                                              },
                                          },
                                          texSpec.clearColor);
                               clearMask |= GL_COLOR_BUFFER_BIT;
                           } else {
                               // other color outputs
                               // Check the color type
                               std::visit(Overloaded{
                                              [&](const FixedClearColor &colInd) {
                                                  switch (colInd) {
                                                  case FixedClearColor::Garbage:
                                                      // do nothing
                                                      break;
                                                  case FixedClearColor::Default:
                                                      // default is black
                                                      glClearBufferfv(GL_COLOR, attachmentIndex,
                                                                      (const GLfloat[]){0.0f, 0.0f, 0.0f, 0.0f});
                                                      break;
                                                  }
                                              },
                                              [&](const glm::vec4 &color) {
                                                  // use the vector
                                                  glClearBufferfv(GL_COLOR, attachmentIndex,
                                                                  &color[0]);
                                              },
                                          },
                                          texSpec.clearColor);
                           }

                           attachmentIndex++;
                       },
                   },
                   texSpec.shaderComponent);
    }

    glClear(clearMask);

    frame.exitRender();

    // wait (for profiling)
#ifdef VITRAE_ENABLE_DETERMINISTIC_RENDERING
    {
        MMETER_SCOPE_PROFILER("Waiting for GL operations");

        glFinish();
    }
#endif
}

void OpenGLComposeClearRender::prepareRequiredLocalAssets(RenderComposeContext ctx) const {}

StringView OpenGLComposeClearRender::getFriendlyName() const
{
    return m_friendlyName;
}

} // namespace Vitrae