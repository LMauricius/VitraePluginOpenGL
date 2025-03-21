#include "VitraePluginOpenGL/Setup.hpp"

#include "VitraePluginOpenGL/Specializations/Renderer.hpp"

namespace VitraePluginOpenGL
{

void setup(Vitrae::ComponentRoot &root)
{
    root.setComponent<Vitrae::Renderer>(new Vitrae::OpenGLRenderer(root));
}

} // namespace VitraePluginOpenGL