#include "VitraePluginOpenGL/Specializations/FrameStore.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Data/Overloaded.hpp"
#include "Vitrae/Params/ParamList.hpp"
#include "Vitrae/Params/Standard.hpp"
#include "VitraePluginOpenGL/Specializations/Renderer.hpp"
#include "VitraePluginOpenGL/Specializations/Texture.hpp"

#include "MMeter.h"

#include "dynasma/standalone.hpp"

namespace
{

static const GLenum drawBufferConstantsOrdered[] = {
    GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,  GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,
    GL_COLOR_ATTACHMENT8,  GL_COLOR_ATTACHMENT9,  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
    GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15,
};

}

namespace Vitrae
{
OpenGLFrameStore::OpenGLFrameStore(const FrameStore::TextureBindParams &params)
    : m_outputTextureSpecs(params.outputTextureSpecs), m_colorAttachmentUnusedIndex(0)
{

    GLuint glFramebufferId;

    glGenFramebuffers(1, &glFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, glFramebufferId);

    int width = 0, height = 0;

    for (const auto &texSpec : params.outputTextureSpecs) {
        if (!texSpec.p_texture.has_value()) {
            continue;
        }

        auto p_texture = dynasma::dynamic_pointer_cast<OpenGLTexture>(texSpec.p_texture.value());

        if (width == 0 && height == 0) {
            width = p_texture->getSize().x;
            height = p_texture->getSize().y;
        } else if (width != p_texture->getSize().x || height != p_texture->getSize().y) {
            throw std::runtime_error("All textures must be the same size");
        }

        GLenum attachment;
        std::visit(Overloaded{
                       [&](const FixedRenderComponent &comp) {
                           switch (comp) {
                           case FixedRenderComponent::Depth:
                               attachment = GL_DEPTH_ATTACHMENT;
                               break;
                           }
                       },
                       [&](const ParamSpec &spec) {
                           attachment = GL_COLOR_ATTACHMENT0 + m_colorAttachmentUnusedIndex;

                           m_renderComponents.emplace_back(spec);

                           m_colorAttachmentUnusedIndex++;
                       },
                   },
                   texSpec.shaderComponent);

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, p_texture->glTextureId,
                               0);
    }

    glDrawBuffers(m_colorAttachmentUnusedIndex, drawBufferConstantsOrdered);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    String glLabel = String("FB ") + String(params.friendlyName);
    glObjectLabel(GL_FRAMEBUFFER, glFramebufferId, glLabel.size(), glLabel.data());

    m_contextSwitcher = FramebufferContextSwitcher{width, height, glFramebufferId};
    mp_renderComponents = dynasma::makeStandalone<ParamList, const std::vector<Vitrae::ParamSpec>>(
        m_renderComponents);
}

OpenGLFrameStore::OpenGLFrameStore(const WindowDisplayParams &params)
    : m_colorAttachmentUnusedIndex(0)
{
    OpenGLRenderer &rend = static_cast<OpenGLRenderer &>(params.root.getComponent<Renderer>());

    GLFWwindow *window = rend.getWindow();

    // reset window
    glfwSetWindowSize(window, params.width, params.height);
    glfwSetWindowTitle(window, params.title.c_str());

    // 'empty' textures just for clearing and stuff
    m_outputTextureSpecs.emplace_back(OutputTextureSpec{
        .p_texture = std::nullopt,
        .shaderComponent = StandardParam::fragment_color,
        .clearColor = params.clearColor,
    });
    m_outputTextureSpecs.emplace_back(OutputTextureSpec{
        .p_texture = std::nullopt,
        .shaderComponent = FixedRenderComponent::Depth,
        .clearColor = params.clearDepth,
    });

    // setup members
    mp_renderComponents = dynasma::makeStandalone<ParamList, std::span<const Vitrae::ParamSpec>>(
        {{StandardParam::fragment_color}});
    m_contextSwitcher = WindowContextSwitcher{window, params.onClose, params.onDrag};

    // register callbacks
    glfwSetWindowUserPointer(window, &std::get<WindowContextSwitcher>(m_contextSwitcher));

    glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));
        switcher->onClose();
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));

        if (switcher->bLeft || switcher->bRight || switcher->bMiddle) {

            switcher->onDrag(glm::vec2(xpos, ypos) - switcher->lastPos, switcher->bLeft,
                             switcher->bRight, switcher->bMiddle);
        }

        switcher->lastPos = glm::vec2(xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            switcher->bLeft = action != GLFW_RELEASE;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            switcher->bRight = action != GLFW_RELEASE;
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            switcher->bMiddle = action != GLFW_RELEASE;
        }
    });
}

OpenGLFrameStore::~OpenGLFrameStore()
{
    std::visit([](auto &contextSwitcher) { contextSwitcher.destroyContext(); }, m_contextSwitcher);
}

std::size_t OpenGLFrameStore::memory_cost() const
{
    return sizeof(OpenGLFrameStore);
}
void OpenGLFrameStore::resize(glm::vec2 size)
{
    /// TODO: resize capabilities
}

void OpenGLFrameStore::bindOutput(const OutputTextureSpec &texSpec)
{
    m_outputTextureSpecs.push_back(texSpec);

    std::visit(
        Overloaded{
            [&](FramebufferContextSwitcher &contextSwitcher) {
                glBindFramebuffer(GL_FRAMEBUFFER, contextSwitcher.glFramebufferId);

                if (!texSpec.p_texture.has_value()) {
                    return;
                }

                auto p_texture =
                    dynasma::dynamic_pointer_cast<OpenGLTexture>(texSpec.p_texture.value());

                if (contextSwitcher.width != p_texture->getSize().x ||
                    contextSwitcher.height != p_texture->getSize().y) {
                    throw std::runtime_error("All bound textures must be the same size");
                }

                GLenum attachment;
                std::visit(Overloaded{
                               [&](const FixedRenderComponent &comp) {
                                   switch (comp) {
                                   case FixedRenderComponent::Depth:
                                       attachment = GL_DEPTH_ATTACHMENT;
                                       break;
                                   }
                               },
                               [&](const ParamSpec &spec) {
                                   attachment = GL_COLOR_ATTACHMENT0 + m_colorAttachmentUnusedIndex;

                                   m_renderComponents.emplace_back(spec);

                                   m_colorAttachmentUnusedIndex++;
                               },
                           },
                           texSpec.shaderComponent);

                glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D,
                                       p_texture->glTextureId, 0);

                // re-setup outputs
                mp_renderComponents =
                    dynasma::makeStandalone<ParamList, const std::vector<Vitrae::ParamSpec>>(
                        m_renderComponents);

                // re-setup the buffer
                glDrawBuffers(m_colorAttachmentUnusedIndex, drawBufferConstantsOrdered);
                glReadBuffer(GL_NONE);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            },
            [&](WindowContextSwitcher &contextSwitcher) {
                throw std::runtime_error("Can't bind texture outputs to a window FrameStore");
            },
        },
        m_contextSwitcher);
}

glm::vec2 OpenGLFrameStore::getSize() const
{
    return std::visit([](auto &contextSwitcher) { return contextSwitcher.getSize(); },
                      m_contextSwitcher);
}

dynasma::FirmPtr<const ParamList> OpenGLFrameStore::getRenderComponents() const
{
    return mp_renderComponents;
}

std::span<const FrameStore::OutputTextureSpec> OpenGLFrameStore::getOutputTextureSpecs() const
{
    return m_outputTextureSpecs;
}

void OpenGLFrameStore::sync(bool vsync)
{
    MMETER_SCOPE_PROFILER("OpenGLFrameStore::sync");

    std::visit([&](auto &contextSwitcher) { contextSwitcher.sync(vsync); }, m_contextSwitcher);
}

void OpenGLFrameStore::enterRender(glm::vec2 topLeft, glm::vec2 bottomRight)
{
    std::visit([&](auto &contextSwitcher) { contextSwitcher.enterContext(topLeft, bottomRight); },
               m_contextSwitcher);
}

void OpenGLFrameStore::exitRender()
{
    std::visit([](auto &contextSwitcher) { contextSwitcher.exitContext(); }, m_contextSwitcher);
}

/*
Framebuffer drawing
*/

void OpenGLFrameStore::FramebufferContextSwitcher::destroyContext()
{
    glDeleteFramebuffers(1, &glFramebufferId);
}
glm::vec2 OpenGLFrameStore::FramebufferContextSwitcher::getSize() const
{
    return glm::vec2(width, height);
}
void OpenGLFrameStore::FramebufferContextSwitcher::enterContext(glm::vec2 topLeft,
                                                                glm::vec2 bottomRight)
{
    glBindFramebuffer(GL_FRAMEBUFFER, glFramebufferId);
    glViewport(topLeft.x * width, topLeft.y * height, (bottomRight.x - topLeft.x) * width,
               (bottomRight.y - topLeft.y) * height);
}
void OpenGLFrameStore::FramebufferContextSwitcher::exitContext()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameStore::FramebufferContextSwitcher::sync(bool vsync) {}

/*
Window drawing
*/

void OpenGLFrameStore::WindowContextSwitcher::destroyContext() {}
glm::vec2 OpenGLFrameStore::WindowContextSwitcher::getSize() const
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return glm::vec2(width, height);
}
void OpenGLFrameStore::WindowContextSwitcher::enterContext(glm::vec2 topLeft, glm::vec2 bottomRight)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(topLeft.x * width, topLeft.y * height, (bottomRight.x - topLeft.x) * width,
               (bottomRight.y - topLeft.y) * height);
}
void OpenGLFrameStore::WindowContextSwitcher::exitContext() {}

void OpenGLFrameStore::WindowContextSwitcher::sync(bool vsync)
{
    glfwSwapInterval(vsync ? 1 : 0);

    glfwSwapBuffers(window);

    // wait (for profiling)
#ifdef VITRAE_ENABLE_DETERMINISTIC_RENDERING
    {
        MMETER_SCOPE_PROFILER("Waiting for GL operations");

        glFinish();
    }
#endif
}

} // namespace Vitrae