#pragma once

#include "Vitrae/Assets/Texture.hpp"
#include "VitraePluginOpenGL/Specializations/PixelImage.hpp"
#include "dynasma/pointer.hpp"
#include "dynasma/standalone.hpp"
#include "glad/glad.h"

namespace Vitrae
{

// ==== Texture mix-ins ============================================================================

template <PixelType TPIXEL_TYPE>
class OpenGLTexture1D : public dynasma::PtrAwareBase, public Texture1D<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLPixelImage1D<TPIXEL_TYPE>;

    OpenGLTexture1D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture1D<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture1D<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture1D<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return p_buffer->getPixelFormat();
    }

    unsigned int getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture1D ----------------------------------------

    dynasma::PinPtr<PixelImage1D<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const PixelImage1D<TPIXEL_TYPE>> getBuffer() const override { return p_buffer; }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture2D : public dynasma::PtrAwareBase, public Texture2D<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLPixelImage2D<TPIXEL_TYPE>;

    OpenGLTexture2D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture2D<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture2D<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture2D<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return p_buffer->getPixelFormat();
    }

    glm::uvec2 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture2D ----------------------------------------

    dynasma::PinPtr<PixelImage2D<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const PixelImage2D<TPIXEL_TYPE>> getBuffer() const override { return p_buffer; }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture3D : public dynasma::PtrAwareBase, public Texture3D<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLPixelImage3D<TPIXEL_TYPE>;

    OpenGLTexture3D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture3D<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture3D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture3D<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture3D(dynasma::PolymorphicReferenceCounter *p_ctr,
                    const Texture3D<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return p_buffer->getPixelFormat();
    }

    glm::uvec3 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture3D ----------------------------------------

    dynasma::PinPtr<PixelImage3D<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const PixelImage3D<TPIXEL_TYPE>> getBuffer() const override { return p_buffer; }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTextureCube : public dynasma::PtrAwareBase, public TextureCubemap<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLPixelImageCube<TPIXEL_TYPE>;

    OpenGLTextureCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                      const TextureCubemap<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                      const TextureCubemap<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                      const TextureCubemap<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return p_buffer->getPixelFormat();
    }

    glm::uvec3 getSize() const override { return p_buffer->getSize(); }

    // ---- For TextureCubemap ----------------------------------------

    dynasma::PinPtr<Texture2D<TPIXEL_TYPE>> getFace(Side side) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const Texture2D<TPIXEL_TYPE>> getFace(Side side) const override
    {
        /// TODO: something
    }

    dynasma::PinPtr<PixelImageCubemap<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const PixelImageCubemap<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture1DLayered : public dynasma::PtrAwareBase, public Texture1DLayered<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLPixelImage1DLayered<TPIXEL_TYPE>;

    OpenGLTexture1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const Texture1DLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const Texture1DLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const Texture1DLayered<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return p_buffer->getPixelFormat();
    }

    glm::uvec2 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture1DLayered ----------------------------------------

    dynasma::PinPtr<Texture1D<TPIXEL_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const Texture1D<TPIXEL_TYPE>> getLayer(std::size_t layer) const override
    {
        /// TODO: something
    }

    dynasma::PinPtr<PixelImage1DLayered<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const PixelImage1DLayered<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture2DLayered : public dynasma::PtrAwareBase, public Texture2DLayered<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLPixelImage2DLayered<TPIXEL_TYPE>;

    OpenGLTexture2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const Texture2DLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const Texture2DLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const Texture2DLayered<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return p_buffer->getPixelFormat();
    }

    glm::uvec3 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture2DLayered ----------------------------------------

    dynasma::PinPtr<Texture2D<TPIXEL_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const Texture2D<TPIXEL_TYPE>> getLayer(std::size_t layer) const override
    {
        /// TODO: something
    }

    dynasma::PinPtr<PixelImage2DLayered<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const PixelImage2DLayered<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTextureCubeLayered : public dynasma::PtrAwareBase,
                                 public TextureCubemapLayered<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLPixelImageCubeLayered<TPIXEL_TYPE>;

    OpenGLTextureCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                             const TextureCubemapLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                             const TextureCubemapLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                             const TextureCubemapLayered<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return p_buffer->getPixelFormat();
    }

    glm::uvec4 getSize() const override { return p_buffer->getSize(); }

    // ---- For TextureCubemapLayered ----------------------------------------

    dynasma::PinPtr<TextureCubemap<TPIXEL_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const TextureCubemap<TPIXEL_TYPE>> getLayer(std::size_t layer) const override
    {
        /// TODO: something
    }

    dynasma::PinPtr<PixelImageCubemapLayered<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const PixelImageCubemapLayered<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

// ==== Texture info ===============================================================================

template <class TextureT> struct OpenGLTextureInfo;

template <PixelType PT> struct OpenGLTextureInfo<Texture1D<PT>>
{
    constexpr static GLint TEXTURE_TARGET = GL_TEXTURE_1D;
};

template <PixelType PT> struct OpenGLTextureInfo<Texture2D<PT>>
{
    constexpr static GLint TEXTURE_TARGET = GL_TEXTURE_2D;
};

template <PixelType PT> struct OpenGLTextureInfo<Texture3D<PT>>
{
    constexpr static GLint TEXTURE_TARGET = GL_TEXTURE_3D;
};

template <PixelType PT> struct OpenGLTextureInfo<TextureCubemap<PT>>
{
    constexpr static GLint TEXTURE_TARGET = GL_TEXTURE_CUBE_MAP;
};

template <PixelType PT> struct OpenGLTextureInfo<Texture1DLayered<PT>>
{
    constexpr static GLint TEXTURE_TARGET = GL_TEXTURE_1D_ARRAY;
};

template <PixelType PT> struct OpenGLTextureInfo<Texture2DLayered<PT>>
{
    constexpr static GLint TEXTURE_TARGET = GL_TEXTURE_2D_ARRAY;
};

template <PixelType PT> struct OpenGLTextureInfo<TextureCubemapLayered<PT>>
{
    constexpr static GLint TEXTURE_TARGET = GL_TEXTURE_CUBE_MAP_ARRAY;
};

} // namespace Vitrae