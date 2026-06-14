#pragma once

#include "Vitrae/Assets/TensorBuffer.hpp"
#include "Vitrae/Assets/Texture.hpp"
#include "Vitrae/Data/PixelTyping.hpp"
#include "Vitrae/Setup/ImageLoad.hpp"
#include "Vitrae/Setup/TextureFiltering.hpp"
#include "Vitrae/Util/Vectors.hpp"
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

    void setupFromFile(PixelType pixelType, const ImageCommon::FileLoadParams &params);
    void setupFromColor(AnyPixelFormat pixelFormat, const void *pixelData);
    void setupEmpty(const glm::uvec4 &ndSize, AnyPixelFormat pixelFormat,
                    const glm::vec<4, ChannelSource> &swizzleSpec,
                    const TextureFilteringParams &filtering, String friendlyName);

    virtual void setPixelFormat(AnyPixelFormat pixelFormat) = 0;
    // void readFile(const ImageCommon::FileLoadParams &params);
    // void createTexture();
    // void loadFiltering(const GpuFormatInfo &formatInfo, const TextureFilteringParams &filtering);
    virtual void setSize(int width, int height, int count) = 0;
    virtual void setSize(glm::uvec4 ndSize) = 0;
    virtual void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) = 0;

  private:
};

// ==== TensorBuffer typed bits ==============================================================

template <PixelType BT> class OpenGLTensorPixelTypedBit
{
  public:
    CompatiblePixelFormat<BT> m_pixelFormat;

  public:
    virtual void setPixelFormatBit(AnyPixelFormat pixelFormat) {}
    virtual CompatiblePixelFormat<BT> getPixelFormatBit() const { return m_pixelFormat; }
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

template <PixelType TPIXEL_TYPE>
class OpenGLTensorBuffer1D : public TensorBuffer1D<TPIXEL_TYPE>,
                             public OpenGLTensorBuffer1DBit,
                             public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLTensorBuffer1D(const Texture1D<TPIXEL_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLTensorBuffer1D(const Texture1D<TPIXEL_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer1D(const Texture1D<TPIXEL_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    unsigned int getSize() const override { return getSizeBit(); }
    void setPixelFormat(AnyPixelFormat pixelFormat) override { setPixelFormatBit(pixelFormat); }
    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return getPixelFormatBit();
    }
};
template <PixelType TPIXEL_TYPE>
class OpenGLTensorBuffer2D : public TensorBuffer2D<TPIXEL_TYPE>,
                             public OpenGLTensorBuffer2DBit,
                             public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLTensorBuffer2D(const Texture2D<TPIXEL_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLTensorBuffer2D(const Texture2D<TPIXEL_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer2D(const Texture2D<TPIXEL_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec2 getSize() const override { return getSizeBit(); }
    void setPixelFormat(AnyPixelFormat pixelFormat) override { setPixelFormatBit(pixelFormat); }
    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return getPixelFormatBit();
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLTensorBuffer3D : public TensorBuffer3D<TPIXEL_TYPE>,
                             public OpenGLTensorBuffer3DBit,
                             public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLTensorBuffer3D(const Texture3D<TPIXEL_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLTensorBuffer3D(const Texture3D<TPIXEL_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer3D(const Texture3D<TPIXEL_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec3 getSize() const override { return getSizeBit(); }
    void setPixelFormat(AnyPixelFormat pixelFormat) override { setPixelFormatBit(pixelFormat); }
    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return getPixelFormatBit();
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLTensorBufferCube : public dynasma::PtrAwareBase,
                               public TensorBufferCubemap<TPIXEL_TYPE>,
                               public OpenGLTensorBufferCubeBit,
                               public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLTensorBufferCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const TextureCubemap<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLTensorBufferCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const TextureCubemap<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, params.size, 6, 0}, params.storageFormat,
                   promotePadVector(params.swizzle), params.filtering, params.friendlyName);
    }
    OpenGLTensorBufferCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                           const TextureCubemap<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec3 getSize() const override { return getSizeBit(); }
    void setPixelFormat(AnyPixelFormat pixelFormat) override { setPixelFormatBit(pixelFormat); }
    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return getPixelFormatBit();
    }

    // For TensorBufferCubemap

    dynasma::IndirectPtr<TensorBuffer2D<TPIXEL_TYPE>> getFace(Side side) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBuffer2D<TPIXEL_TYPE>> getFace(Side side) const override
    {
        /// TODO: something
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLTensorBuffer1DLayered : public dynasma::PtrAwareBase,
                                    public TensorBuffer1DLayered<TPIXEL_TYPE>,
                                    public OpenGLTensorBuffer1DLayeredBit,
                                    public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLTensorBuffer1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const Texture1DLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLTensorBuffer1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const Texture1DLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const Texture1DLayered<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec2 getSize() const override { return getSizeBit(); }
    void setPixelFormat(AnyPixelFormat pixelFormat) override { setPixelFormatBit(pixelFormat); }
    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return getPixelFormatBit();
    }

    // For TensorBuffer1DLayered

    dynasma::IndirectPtr<TensorBuffer1D<TPIXEL_TYPE>> getLayer(std::size_t y) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBuffer1D<TPIXEL_TYPE>> getLayer(std::size_t y) const override
    {
        /// TODO: something
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLTensorBuffer2DLayered : public dynasma::PtrAwareBase,
                                    public TensorBuffer2DLayered<TPIXEL_TYPE>,
                                    public OpenGLTensorBuffer2DLayeredBit,
                                    public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLTensorBuffer2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const Texture2DLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLTensorBuffer2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const Texture2DLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLTensorBuffer2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const Texture2DLayered<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec3 getSize() const override { return getSizeBit(); }
    void setPixelFormat(AnyPixelFormat pixelFormat) override { setPixelFormatBit(pixelFormat); }
    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return getPixelFormatBit();
    }

    // For TensorBuffer2DLayered

    dynasma::IndirectPtr<TensorBuffer2D<TPIXEL_TYPE>> getLayer(std::size_t z) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBuffer2D<TPIXEL_TYPE>> getLayer(std::size_t z) const override
    {
        /// TODO: something
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLTensorBufferCubeLayered : public dynasma::PtrAwareBase,
                                      public TensorBufferCubemapLayered<TPIXEL_TYPE>,
                                      public OpenGLTensorBufferCubeLayeredBit,
                                      public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLTensorBufferCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                  const TextureCubemapLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLTensorBufferCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                  const TextureCubemapLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size.x, params.size.x, 6, params.size.y}, params.storageFormat,
                   promotePadVector(params.swizzle), params.filtering, params.friendlyName);
    }
    OpenGLTensorBufferCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                  const TextureCubemapLayered<TPIXEL_TYPE>::PureColorParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromColor(params.storageFormat, &params.color);
    }

    std::size_t memory_cost() const override { return sizeof(*this) + gpuMemoryCostBit(); }
    glm::uvec4 getSize() const override { return getSizeBit(); }
    void setPixelFormat(AnyPixelFormat pixelFormat) override { setPixelFormatBit(pixelFormat); }
    CompatiblePixelFormat<TPIXEL_TYPE> getPixelFormat() const override
    {
        return getPixelFormatBit();
    }

    // For TensorBufferCubemapLayered

    dynasma::IndirectPtr<TensorBufferCubemap<TPIXEL_TYPE>> getLayer(std::size_t z) override
    {
        /// TODO: something
    }

    dynasma::IndirectPtr<const TensorBufferCubemap<TPIXEL_TYPE>> getLayer(
        std::size_t z) const override
    {
        /// TODO: something
    }
};

} // namespace Vitrae