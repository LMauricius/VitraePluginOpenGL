#pragma once

#include "Vitrae/Assets/Texture.hpp"
#include "VitraePluginOpenGL/Specializations/TensorBuffer.hpp"
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
    using BufferT = OpenGLTensorBuffer1D<TPIXEL_TYPE>;

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

    dynasma::PinPtr<TensorBuffer1D<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const TensorBuffer1D<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture2D : public dynasma::PtrAwareBase, public Texture2D<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer2D<TPIXEL_TYPE>;

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

    dynasma::PinPtr<TensorBuffer2D<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const TensorBuffer2D<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture3D : public dynasma::PtrAwareBase, public Texture3D<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer3D<TPIXEL_TYPE>;

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

    dynasma::PinPtr<TensorBuffer3D<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const TensorBuffer3D<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTextureCube : public dynasma::PtrAwareBase, public TextureCubemap<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLTensorBufferCube<TPIXEL_TYPE>;

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

    dynasma::PinPtr<TensorBufferCubemap<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const TensorBufferCubemap<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture1DLayered : public dynasma::PtrAwareBase, public Texture1DLayered<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer1DLayered<TPIXEL_TYPE>;

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

    dynasma::PinPtr<TensorBuffer1DLayered<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const TensorBuffer1DLayered<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <PixelType TPIXEL_TYPE>
class OpenGLTexture2DLayered : public dynasma::PtrAwareBase, public Texture2DLayered<TPIXEL_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer2DLayered<TPIXEL_TYPE>;

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

    dynasma::PinPtr<TensorBuffer2DLayered<TPIXEL_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::PinPtr<const TensorBuffer2DLayered<TPIXEL_TYPE>> getBuffer() const override
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
    using BufferT = OpenGLTensorBufferCubeLayered<TPIXEL_TYPE>;

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

    dynasma::PinPtr<TensorBufferCubemapLayered<TPIXEL_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::PinPtr<const TensorBufferCubemapLayered<TPIXEL_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

} // namespace Vitrae