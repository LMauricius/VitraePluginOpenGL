#pragma once

#include "Vitrae/Assets/BufferUtil/Ptr.hpp"
#include "Vitrae/Data/StringId.hpp"
#include "Vitrae/Renderer.hpp"

#include "glad/glad.h"
// must be after glad.h
#include "GLFW/glfw3.h"

#include <cstddef>
#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <thread>
#include <typeindex>
#include <vector>

namespace Vitrae
{
class Mesh;
class Texture;
class RawSharedBuffer;
class ComposeTask;

struct GLLayoutSpec
{
    std::size_t std140Size;
    std::size_t std140Alignment;
    std::size_t indexSize;
};

struct GLTypeSpec
{
    String valueTypeName;
    String opaqueTypeName;
    String structBodySnippet;

    GLLayoutSpec layout;

    // used only if the type is a struct and has a flexible array member
    struct FlexibleMemberSpec
    {
        const GLTypeSpec &elementGlTypeSpec;
        String memberName;
        std::size_t maxNumElements;
    };
    std::optional<FlexibleMemberSpec> flexibleMemberSpec;

    std::vector<const GLTypeSpec *> memberTypeDependencies;
};

struct GLScalarSpec
{

    /**
     * Constant such as GL_UNSIGNED_BYTE, GL_FLOAT etc.
     */
    GLenum glTypeId;

    /**
     * Whether the value is a normalized integer that maps to [0.0, 1.0] or [-1.0, 1.0]
     */
    bool isNormalized;
};

struct GLConversionSpec
{
    const GLTypeSpec &glTypeSpec;

    std::optional<GLScalarSpec> scalarSpec;

    std::function<void(GLint location, const Variant &hostValue)>   setUniform       = nullptr;
    std::function<void(int bindingIndex, const Variant &hostValue)> setOpaqueBinding = nullptr;
    std::function<void(int bindingIndex, const Variant &hostValue)> setUBOBinding    = nullptr;
    std::function<void(int bindingIndex, const Variant &hostValue)> setSSBOBinding   = nullptr;
};

/**
 * @brief Thrown when there is an error in type specification
 */
struct GLSpecificationError : public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

class OpenGLRenderer : public Renderer
{
  public:
    OpenGLRenderer(ComponentRoot &root);
    ~OpenGLRenderer();

    void mainThreadSetup(ComponentRoot &root) override;
    void mainThreadFree() override;
    void mainThreadUpdate() override;

    void anyThreadEnable() override;
    void anyThreadDisable() override;

    GLFWwindow *getWindow();

    /**
     * @brief Register needed GL info about a type automagically using TypeMeta
     */
    void registerTypeAuto(const TypeInfo &hostType);

    /**
     * @brief Specify a GL type
     * @returns a reference to the permanent type specification struct
     * @note The returned struct is valid until the end of the renderer's lifetime
     */
    const GLTypeSpec &specifyGlType(GLTypeSpec &&newSpec);

    /**
     * @brief Registers a native to GL type conversion
     * @returns a reference to the permanent type conversion struct
     * @param hostType the native type
     * @param newSpec the conversion specification
     * @note The returned struct is valid until the end of the renderer's lifetime
     */
    const GLConversionSpec &registerTypeConversion(const TypeInfo &hostType,
                                                   GLConversionSpec &&newSpec);
    const GLConversionSpec &getTypeConversion(const TypeInfo &hostType);

    void specifyVertexBuffer(const ParamSpec &newElSpec) override;
    void specifyTextureSampler(StringView colorName) override;

    std::size_t getNumVertexBuffers() const;
    std::size_t getVertexBufferLayoutIndex(StringId name) const;
    const StableMap<StringId, const GLTypeSpec *> &getAllVertexBufferSpecs() const;

  protected:
    ComponentRoot &m_root;

    std::thread::id m_mainThreadId;
    std::mutex m_contextMutex;
    GLFWwindow *mp_mainWindow;

    std::deque<GLTypeSpec> m_glTypes;
    std::deque<GLConversionSpec> m_glConversions;
    StableMap<std::type_index, GLConversionSpec *> m_glConversionsByHostType;

    StableMap<StringId, std::size_t> m_vertexBufferIndices;
    std::size_t m_vertexBufferFreeIndex;
    StableMap<StringId, const GLTypeSpec *> m_vertexBufferSpecs;

    std::unordered_set<StringId> m_specifiedColorNames;

    mutable StableMap<std::size_t, StableMap<StringId, ParamSpec>> m_sceneRenderInputDependencies;

    // utility
    static void setRawBufferBinding(const RawSharedBuffer &buf, int bindingIndex);
};

} // namespace Vitrae