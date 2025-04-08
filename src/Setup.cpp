#include "VitraePluginOpenGL/Setup.hpp"

#include "Vitrae/Assets/Material.hpp"

#include "VitraePluginOpenGL/Specializations/Renderer.hpp"

#include "VitraePluginOpenGL/Specializations/Compositing/ClearRender.hpp"
#include "VitraePluginOpenGL/Specializations/Compositing/Compute.hpp"
#include "VitraePluginOpenGL/Specializations/Compositing/DataRender.hpp"
#include "VitraePluginOpenGL/Specializations/Compositing/IndexRender.hpp"
#include "VitraePluginOpenGL/Specializations/Compositing/SceneRender.hpp"
#include "VitraePluginOpenGL/Specializations/FrameStore.hpp"
#include "VitraePluginOpenGL/Specializations/Mesh.hpp"
#include "VitraePluginOpenGL/Specializations/ShaderCompilation.hpp"
#include "VitraePluginOpenGL/Specializations/Shading/Constant.hpp"
#include "VitraePluginOpenGL/Specializations/Shading/FunctionCall.hpp"
#include "VitraePluginOpenGL/Specializations/Shading/Header.hpp"
#include "VitraePluginOpenGL/Specializations/Shading/Snippet.hpp"
#include "VitraePluginOpenGL/Specializations/SharedBuffer.hpp"
#include "VitraePluginOpenGL/Specializations/Texture.hpp"

#include "dynasma/cachers/basic.hpp"
#include "dynasma/keepers/naive.hpp"
#include "dynasma/managers/basic.hpp"

namespace VitraePluginOpenGL
{

void setup(Vitrae::ComponentRoot &root)
{
    using namespace Vitrae;

    root.setComponent<Renderer>(new OpenGLRenderer(root));

    // clang-format off
    root.setComponent<              MeshKeeper>(new  dynasma::NaiveKeeper<              MeshKeeperSeed, std::allocator<              OpenGLMesh>>());
    root.setComponent<          MaterialKeeper>(new  dynasma::NaiveKeeper<          MaterialKeeperSeed, std::allocator<                Material>>());
    root.setComponent<          TextureManager>(new dynasma::BasicManager<                 TextureSeed, std::allocator<           OpenGLTexture>>());
    root.setComponent<       FrameStoreManager>(new dynasma::BasicManager<              FrameStoreSeed, std::allocator<        OpenGLFrameStore>>());
    root.setComponent<   RawSharedBufferKeeper>(new  dynasma::NaiveKeeper<   RawSharedBufferKeeperSeed, std::allocator<   OpenGLRawSharedBuffer>>());
    root.setComponent<    ShaderConstantKeeper>(new  dynasma::NaiveKeeper<    ShaderConstantKeeperSeed, std::allocator<    OpenGLShaderConstant>>());
    root.setComponent<     ShaderSnippetKeeper>(new  dynasma::NaiveKeeper<     ShaderSnippetKeeperSeed, std::allocator<     OpenGLShaderSnippet>>());
    root.setComponent<ShaderFunctionCallKeeper>(new  dynasma::NaiveKeeper<ShaderFunctionCallKeeperSeed, std::allocator<OpenGLShaderFunctionCall>>());
    root.setComponent<      ShaderHeaderKeeper>(new  dynasma::NaiveKeeper<      ShaderHeaderKeeperSeed, std::allocator<      OpenGLShaderHeader>>());
    root.setComponent<ComposeSceneRenderKeeper>(new  dynasma::NaiveKeeper<ComposeSceneRenderKeeperSeed, std::allocator<OpenGLComposeSceneRender>>());
    root.setComponent< ComposeDataRenderKeeper>(new  dynasma::NaiveKeeper< ComposeDataRenderKeeperSeed, std::allocator< OpenGLComposeDataRender>>());
    root.setComponent<ComposeIndexRenderKeeper>(new  dynasma::NaiveKeeper<ComposeIndexRenderKeeperSeed, std::allocator<OpenGLComposeIndexRender>>());
    root.setComponent<    ComposeComputeKeeper>(new  dynasma::NaiveKeeper<    ComposeComputeKeeperSeed, std::allocator<    OpenGLComposeCompute>>());
    root.setComponent<ComposeClearRenderKeeper>(new  dynasma::NaiveKeeper<ComposeClearRenderKeeperSeed, std::allocator<OpenGLComposeClearRender>>());
    root.setComponent<CompiledGLSLShaderCacher>(new  dynasma::BasicCacher<CompiledGLSLShaderCacherSeed, std::allocator<      CompiledGLSLShader>>());
    // clang-format on
}

} // namespace VitraePluginOpenGL