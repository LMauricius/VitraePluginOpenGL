#pragma once

#include "Vitrae/Assets/Texture.hpp"
#include "VitraePluginOpenGL/Specializations/TensorBuffer.hpp"
#include "dynasma/pointer.hpp"
#include "dynasma/standalone.hpp"
#include "glad/glad.h"

namespace Vitrae
{

// ==== Texture mix-ins ============================================================================

template <BufferType TBUFFER_TYPE>
class OpenGLTexture1D : public dynasma::PtrAwareBase, public Texture1D<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer1D<TBUFFER_TYPE>;

    OpenGLTexture1D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture1D<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture1D<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture1D<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    unsigned int getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture1D ----------------------------------------

    dynasma::IndirectPtr<TensorBuffer1D<TBUFFER_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::IndirectPtr<const TensorBuffer1D<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture2D : public dynasma::PtrAwareBase, public Texture2D<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer2D<TBUFFER_TYPE>;

    OpenGLTexture2D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture2D<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture2D<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture2D<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    glm::uvec2 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture2D ----------------------------------------

    dynasma::IndirectPtr<TensorBuffer2D<TBUFFER_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::IndirectPtr<const TensorBuffer2D<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture3D : public dynasma::PtrAwareBase, public Texture3D<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer3D<TBUFFER_TYPE>;

    OpenGLTexture3D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture3D<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture3D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture3D<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture3D(dynasma::TypeErasedReferenceCounter *p_ctr,
                    const Texture3D<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    glm::uvec3 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture3D ----------------------------------------

    dynasma::IndirectPtr<TensorBuffer3D<TBUFFER_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::IndirectPtr<const TensorBuffer3D<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTextureCube : public dynasma::PtrAwareBase, public TextureCubemap<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBufferCube<TBUFFER_TYPE>;

    OpenGLTextureCube(dynasma::TypeErasedReferenceCounter *p_ctr,
                      const TextureCubemap<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCube(dynasma::TypeErasedReferenceCounter *p_ctr,
                      const TextureCubemap<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCube(dynasma::TypeErasedReferenceCounter *p_ctr,
                      const TextureCubemap<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    glm::uvec3 getSize() const override { return p_buffer->getSize(); }

    // ---- For TextureCubemap ----------------------------------------

    dynasma::IndirectPtr<Texture2D<TBUFFER_TYPE>> getFace(Side side) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const Texture2D<TBUFFER_TYPE>> getFace(Side side) const override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<TensorBufferCubemap<TBUFFER_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::IndirectPtr<const TensorBufferCubemap<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture1DLayered : public dynasma::PtrAwareBase, public Texture1DLayered<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer1DLayered<TBUFFER_TYPE>;

    OpenGLTexture1DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const Texture1DLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const Texture1DLayered<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const Texture1DLayered<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    glm::uvec2 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture1DLayered ----------------------------------------

    dynasma::IndirectPtr<Texture1D<TBUFFER_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const Texture1D<TBUFFER_TYPE>> getLayer(std::size_t layer) const override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<TensorBuffer1DLayered<TBUFFER_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::IndirectPtr<const TensorBuffer1DLayered<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture2DLayered : public dynasma::PtrAwareBase, public Texture2DLayered<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer2DLayered<TBUFFER_TYPE>;

    OpenGLTexture2DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const Texture2DLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const Texture2DLayered<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const Texture2DLayered<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    glm::uvec3 getSize() const override { return p_buffer->getSize(); }

    // ---- For Texture2DLayered ----------------------------------------

    dynasma::IndirectPtr<Texture2D<TBUFFER_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const Texture2D<TBUFFER_TYPE>> getLayer(std::size_t layer) const override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<TensorBuffer2DLayered<TBUFFER_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::IndirectPtr<const TensorBuffer2DLayered<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTextureCubeLayered : public dynasma::PtrAwareBase,
                                 public TextureCubemapLayered<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBufferCubeLayered<TBUFFER_TYPE>;

    OpenGLTextureCubeLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                             const TextureCubemapLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCubeLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                             const TextureCubemapLayered<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCubeLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                             const TextureCubemapLayered<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr), p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    glm::uvec4 getSize() const override { return p_buffer->getSize(); }

    // ---- For TextureCubemapLayered ----------------------------------------

    dynasma::IndirectPtr<TextureCubemap<TBUFFER_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TextureCubemap<TBUFFER_TYPE>> getLayer(
        std::size_t layer) const override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<TensorBufferCubemapLayered<TBUFFER_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::IndirectPtr<const TensorBufferCubemapLayered<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

} // namespace Vitrae