#pragma once

#include "Vitrae/Assets/Texture.hpp"
#include "VitraePluginOpenGL/Specializations/TensorBuffer.hpp"
#include "dynasma/pointer.hpp"
#include "dynasma/standalone.hpp"
#include "glad/glad.h"

namespace Vitrae
{

// ==== Texture mix-ins ============================================================================

template <BufferType TBUFFER_TYPE> class OpenGLTexture1D : public Texture1D<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer1D<TBUFFER_TYPE>;

    OpenGLTexture1D(const Texture1D<TBUFFER_TYPE>::FileLoadParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1D(const Texture1D<TBUFFER_TYPE>::EmptyParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1D(const Texture1D<TBUFFER_TYPE>::PureColorParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
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

    dynasma::SharedPtr<TensorBuffer1D<TBUFFER_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::SharedPtr<const TensorBuffer1D<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE> class OpenGLTexture2D : public Texture2D<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer2D<TBUFFER_TYPE>;

    OpenGLTexture2D(const Texture2D<TBUFFER_TYPE>::FileLoadParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2D(const Texture2D<TBUFFER_TYPE>::EmptyParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2D(const Texture2D<TBUFFER_TYPE>::PureColorParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
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

    dynasma::SharedPtr<TensorBuffer2D<TBUFFER_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::SharedPtr<const TensorBuffer2D<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE> class OpenGLTexture3D : public Texture3D<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer3D<TBUFFER_TYPE>;

    OpenGLTexture3D(const Texture3D<TBUFFER_TYPE>::FileLoadParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture3D(const Texture3D<TBUFFER_TYPE>::EmptyParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture3D(const Texture3D<TBUFFER_TYPE>::PureColorParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
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

    dynasma::SharedPtr<TensorBuffer3D<TBUFFER_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::SharedPtr<const TensorBuffer3D<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE> class OpenGLTextureCube : public TextureCubemap<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBufferCube<TBUFFER_TYPE>;

    OpenGLTextureCube(const TextureCubemap<TBUFFER_TYPE>::FileLoadParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCube(const TextureCubemap<TBUFFER_TYPE>::EmptyParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCube(const TextureCubemap<TBUFFER_TYPE>::PureColorParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
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

    dynasma::SharedPtr<Texture2D<TBUFFER_TYPE>> getFace(Side side) override
    {
        /// TODO: something
    }

    dynasma::SharedPtr<const Texture2D<TBUFFER_TYPE>> getFace(Side side) const override
    {
        /// TODO: something
    }

    dynasma::SharedPtr<TensorBufferCubemap<TBUFFER_TYPE>> getBuffer() override { return p_buffer; }

    dynasma::SharedPtr<const TensorBufferCubemap<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture1DLayered : public Texture1DLayered<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer1DLayered<TBUFFER_TYPE>;

    OpenGLTexture1DLayered(const Texture1DLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1DLayered(const Texture1DLayered<TBUFFER_TYPE>::EmptyParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture1DLayered(const Texture1DLayered<TBUFFER_TYPE>::PureColorParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
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

    dynasma::SharedPtr<Texture1D<TBUFFER_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::SharedPtr<const Texture1D<TBUFFER_TYPE>> getLayer(std::size_t layer) const
    {
        /// TODO: something
    }

    dynasma::SharedPtr<TensorBuffer1DLayered<TBUFFER_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::SharedPtr<const TensorBuffer1DLayered<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture2DLayered : public Texture2DLayered<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBuffer2DLayered<TBUFFER_TYPE>;

    OpenGLTexture2DLayered(const Texture2DLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2DLayered(const Texture2DLayered<TBUFFER_TYPE>::EmptyParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTexture2DLayered(const Texture2DLayered<TBUFFER_TYPE>::PureColorParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
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

    dynasma::SharedPtr<Texture2D<TBUFFER_TYPE>> getLayer(std::size_t layer) override
    {
        /// TODO: something
    }

    dynasma::SharedPtr<const Texture2D<TBUFFER_TYPE>> getLayer(std::size_t layer) const override
    {
        /// TODO: something
    }

    dynasma::SharedPtr<TensorBuffer2DLayered<TBUFFER_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::SharedPtr<const TensorBuffer2DLayered<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

template <BufferType TBUFFER_TYPE>
class OpenGLTextureCubeLayered : public TextureCubemapLayered<TBUFFER_TYPE>
{
  public:
    using BufferT = OpenGLTensorBufferCubeLayered<TBUFFER_TYPE>;

    OpenGLTextureCubeLayered(const TextureCubemapLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCubeLayered(const TextureCubemapLayered<TBUFFER_TYPE>::EmptyParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}
    OpenGLTextureCubeLayered(const TextureCubemapLayered<TBUFFER_TYPE>::PureColorParams &params)
        : p_buffer{dynasma::makeStandalone<BufferT>(params)}
    {}

    // ---- For dynasma ----------------------------------------

    std::size_t memory_cost() const override { return p_buffer->memory_cost() + sizeof(*this); }

    // ---- For TextureBaseTyped ----------------------------------------

    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override
    {
        return p_buffer->getBufferFormat();
    }

    glm::uvec3 getSize() const override { return p_buffer->getSize(); }

    // ---- For TextureCubemapLayered ----------------------------------------

    dynasma::SharedPtr<TensorBufferCubemapLayered<TBUFFER_TYPE>> getBuffer() override
    {
        return p_buffer;
    }

    dynasma::SharedPtr<const TensorBufferCubemapLayered<TBUFFER_TYPE>> getBuffer() const override
    {
        return p_buffer;
    }

    dynasma::FirmPtr<BufferT> p_buffer;
};

} // namespace Vitrae