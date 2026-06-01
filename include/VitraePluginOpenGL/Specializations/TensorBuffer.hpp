#pragma once

#include "Vitrae/Assets/TensorBuffer.hpp"
#include "Vitrae/Assets/Texture.hpp"
#include "Vitrae/Data/BufferFormat.hpp"
#include "Vitrae/Setup/ImageLoad.hpp"
#include "Vitrae/Setup/TextureFiltering.hpp"
#include "Vitrae/Util/Math.hpp"
#include "dynasma/core_concepts.hpp"
#include "dynasma/pointer.hpp"
#include "dynasma/util/dynamic_typing.hpp"
#include "glad/glad.h"

#include <span>

namespace Vitrae
{
class ComponentRoot;
class OpenGLRenderer;

struct GpuFormatInfo
{
    union {
        struct
        {
            GLint r, g, b, a;
        } swizzleVal;
        GLint swizzleArr[4];
    };
    bool useSwizzle;

    GLint internalFormat;
};

struct CpuDataInfo
{
    GLint format;
    GLenum type;
    std::size_t bitsPerElement;
};

struct LoadInfo
{
    CpuDataInfo metaData;
    union {
        void *pixelData;
        const void *constPixelData;
    };
};

/**
 * A TensorBuffer is a single image-like resource
 */
class OpenGLTensorBufferBit
{
  public:
    OpenGLTensorBufferBit();
    virtual ~OpenGLTensorBufferBit();

    std::size_t gpuMemoryCostBit() const;

    GLuint glTextureId;

  protected:
    bool m_sentToGPU;

    void setupFromFile(BufferType bufferType, const ImageCommon::FileLoadParams &params);
    void setupFromColor(AnyBufferFormat bufferFormat, const void *pixelData);
    void setupEmpty(const glm::uvec4 &ndSize, AnyBufferFormat bufferFormat,
                    const glm::vec<4, ChannelSource> &swizzleSpec,
                    const TextureFilteringParams &filtering, String friendlyName);

    virtual void setBufferFormat(AnyBufferFormat bufferFormat) = 0;
    // void readFile(const ImageCommon::FileLoadParams &params);
    // void createTexture();
    // void loadFiltering(const GpuFormatInfo &formatInfo, const TextureFilteringParams &filtering);
    virtual void setSize(int width, int height, int count) = 0;
    virtual void setSize(glm::uvec4 ndSize) = 0;
    virtual void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) = 0;

  private:
};

// ==== TensorBuffer typed bits ==============================================================

template <BufferType BT> class OpenGLTensorBufferTypedBit
{
  public:
    CompatibleBufferFormat<BT> m_bufferFormat;

  public:
    virtual void setFormatBit(AnyBufferFormat bufferFormat) {}
    virtual CompatibleBufferFormat<BT> getFormatBit() const { return m_bufferFormat; }
};

// ==== TensorBuffer dimensioned bits ==============================================================

class OpenGLTensorBuffer1DBit : public OpenGLTensorBufferBit
{
  protected:
    unsigned int getSizeBit() const { return m_size; }
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLTensorBuffer2DBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec2 getSizeBit() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec2 m_size;
};

class OpenGLTensorBuffer3DBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec3 getSizeBit() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec3 m_size;
};

class OpenGLTensorBufferCubeBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec3 getSizeBit() const { return {m_size, m_size, 6}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLTensorBuffer1DLayeredBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec2 getSizeBit() const { return {m_size, m_layerCount}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
    unsigned int m_layerCount;
};

class OpenGLTensorBuffer2DLayeredBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec3 getSizeBit() const { return {m_size, m_layerCount}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec2 m_size;
    unsigned int m_layerCount;
};

class OpenGLTensorBufferCubeLayeredBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec4 getSizeBit() const { return {m_size, m_size, 6, m_layerCount}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
    unsigned int m_layerCount;
};

// ==== TensorBuffer mix-ins =======================================================================

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer1D : public TensorBuffer1D<TBUFFER_TYPE>,
                             public OpenGLTensorBuffer1DBit,
                             public OpenGLTensorBufferTypedBit<TBUFFER_TYPE>
{
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::getFormatBit;
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::setFormatBit;

  public:
    OpenGLTensorBuffer1D(const Texture1D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer1D(const Texture1D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer1D(const Texture1D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    unsigned int getSize() const override { return getSizeBit(); }
    void setBufferFormat(AnyBufferFormat bufferFormat) override { setFormatBit(bufferFormat); }
    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override { getFormatBit(); }
};
template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer2D : public TensorBuffer2D<TBUFFER_TYPE>,
                             public OpenGLTensorBuffer2DBit,
                             public OpenGLTensorBufferTypedBit<TBUFFER_TYPE>
{
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::getFormatBit;
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::setFormatBit;

  public:
    OpenGLTensorBuffer2D(const Texture2D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer2D(const Texture2D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer2D(const Texture2D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec2 getSize() const override { return getSizeBit(); }
    void setBufferFormat(AnyBufferFormat bufferFormat) override { setFormatBit(bufferFormat); }
    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override { getFormatBit(); }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer3D : public TensorBuffer3D<TBUFFER_TYPE>,
                             public OpenGLTensorBuffer3DBit,
                             public OpenGLTensorBufferTypedBit<TBUFFER_TYPE>
{
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::getFormatBit;
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::setFormatBit;

  public:
    OpenGLTensorBuffer3D(const Texture3D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer3D(const Texture3D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer3D(const Texture3D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec3 getSize() const override { return getSizeBit(); }
    void setBufferFormat(AnyBufferFormat bufferFormat) override { setFormatBit(bufferFormat); }
    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override { getFormatBit(); }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBufferCube : public dynasma::PtrAwareBase,
                               public TensorBufferCubemap<TBUFFER_TYPE>,
                               public OpenGLTensorBufferCubeBit,
                               public OpenGLTensorBufferTypedBit<TBUFFER_TYPE>
{
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::getFormatBit;
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::setFormatBit;

  public:
    OpenGLTensorBufferCube(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const TextureCubemap<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBufferCube(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const TextureCubemap<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, params.size, 6, 0}, params.storageFormat,
                   promotePadVector(params.swizzle), params.filtering, params.friendlyName);
    }
    OpenGLTensorBufferCube(dynasma::TypeErasedReferenceCounter *p_ctr,
                           const TextureCubemap<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec3 getSize() const override { return getSizeBit(); }
    void setBufferFormat(AnyBufferFormat bufferFormat) override { setFormatBit(bufferFormat); }
    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override { getFormatBit(); }

    // For TensorBufferCubemap

    dynasma::IndirectPtr<TensorBuffer2D<TBUFFER_TYPE>> getFace(Side side) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBuffer2D<TBUFFER_TYPE>> getFace(Side side) const override
    {
        /// TODO: something
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer1DLayered : public dynasma::PtrAwareBase,
                                    public TensorBuffer1DLayered<TBUFFER_TYPE>,
                                    public OpenGLTensorBuffer1DLayeredBit,
                                    public OpenGLTensorBufferTypedBit<TBUFFER_TYPE>
{
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::getFormatBit;
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::setFormatBit;

  public:
    OpenGLTensorBuffer1DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                const Texture1DLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer1DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                const Texture1DLayered<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer1DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                const Texture1DLayered<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec2 getSize() const override { return getSizeBit(); }
    void setBufferFormat(AnyBufferFormat bufferFormat) override { setFormatBit(bufferFormat); }
    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override { getFormatBit(); }

    // For TensorBuffer1DLayered

    dynasma::IndirectPtr<TensorBuffer1D<TBUFFER_TYPE>> getLayer(std::size_t y) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBuffer1D<TBUFFER_TYPE>> getLayer(std::size_t y) const override
    {
        /// TODO: something
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer2DLayered : public dynasma::PtrAwareBase,
                                    public TensorBuffer2DLayered<TBUFFER_TYPE>,
                                    public OpenGLTensorBuffer2DLayeredBit,
                                    public OpenGLTensorBufferTypedBit<TBUFFER_TYPE>
{
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::getFormatBit;
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::setFormatBit;

  public:
    OpenGLTensorBuffer2DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                const Texture2DLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer2DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                const Texture2DLayered<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer2DLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                const Texture2DLayered<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec3 getSize() const override { return getSizeBit(); }
    void setBufferFormat(AnyBufferFormat bufferFormat) override { setFormatBit(bufferFormat); }
    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override { getFormatBit(); }

    // For TensorBuffer2DLayered

    dynasma::IndirectPtr<TensorBuffer2D<TBUFFER_TYPE>> getLayer(std::size_t z) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBuffer2D<TBUFFER_TYPE>> getLayer(std::size_t z) const override
    {
        /// TODO: something
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBufferCubeLayered : public dynasma::PtrAwareBase,
                                      public TensorBufferCubemapLayered<TBUFFER_TYPE>,
                                      public OpenGLTensorBufferCubeLayeredBit,
                                      public OpenGLTensorBufferTypedBit<TBUFFER_TYPE>
{
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::getFormatBit;
    using OpenGLTensorBufferTypedBit<TBUFFER_TYPE>::setFormatBit;

  public:
    OpenGLTensorBufferCubeLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                  const TextureCubemapLayered<TBUFFER_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBufferCubeLayered(dynasma::TypeErasedReferenceCounter *p_ctr,
                                  const TextureCubemapLayered<TBUFFER_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size.x, params.size.x, 6, params.size.y}, params.storageFormat,
                   promotePadVector(params.swizzle), params.filtering, params.friendlyName);
    }
    OpenGLTensorBufferCubeLayered(
        dynasma::TypeErasedReferenceCounter *p_ctr,
        const TextureCubemapLayered<TBUFFER_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec4 getSize() const override { return getSizeBit(); }
    void setBufferFormat(AnyBufferFormat bufferFormat) override { setFormatBit(bufferFormat); }
    CompatibleBufferFormat<TBUFFER_TYPE> getBufferFormat() const override { getFormatBit(); }

    // For TensorBufferCubemapLayered

    dynasma::IndirectPtr<TensorBufferCubemap<TBUFFER_TYPE>> getLayer(std::size_t z) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBufferCubemap<TBUFFER_TYPE>> getLayer(
        std::size_t z) const override
    {
        /// TODO: something
    }
};

} // namespace Vitrae