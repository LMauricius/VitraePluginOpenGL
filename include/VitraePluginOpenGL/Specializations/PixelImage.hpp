#pragma once

#include "Vitrae/Assets/PixelImage.hpp"
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
 * A PixelImage is a single image-like resource
 */
class OpenGLPixelImageBit
{
  public:
    OpenGLPixelImageBit();
    virtual ~OpenGLPixelImageBit();

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

// ==== PixelImage typed bits ==============================================================

template <PixelType BT> class OpenGLTensorPixelTypedBit
{
  public:
    CompatiblePixelFormat<BT> m_pixelFormat;

  public:
    virtual void setPixelFormatBit(AnyPixelFormat pixelFormat) {}
    virtual CompatiblePixelFormat<BT> getPixelFormatBit() const { return m_pixelFormat; }
};

// ==== PixelImage dimensioned bits ==============================================================

class OpenGLPixelImage1DBit : public OpenGLPixelImageBit
{
  protected:
    unsigned int getSizeBit() const { return m_size; }
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLPixelImage2DBit : public OpenGLPixelImageBit
{
  public:
    glm::uvec2 getSizeBit() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec2 m_size;
};

class OpenGLPixelImage3DBit : public OpenGLPixelImageBit
{
  public:
    glm::uvec3 getSizeBit() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec3 m_size;
};

class OpenGLPixelImageCubeBit : public OpenGLPixelImageBit
{
  public:
    glm::uvec3 getSizeBit() const { return {m_size, m_size, 6}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLPixelImage1DLayeredBit : public OpenGLPixelImageBit
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

class OpenGLPixelImage2DLayeredBit : public OpenGLPixelImageBit
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

class OpenGLPixelImageCubeLayeredBit : public OpenGLPixelImageBit
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

// ==== PixelImage mix-ins =======================================================================

template <PixelType TPIXEL_TYPE>
class OpenGLPixelImage1D : public PixelImage1D<TPIXEL_TYPE>,
                           public OpenGLPixelImage1DBit,
                           public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLPixelImage1D(const Texture1D<TPIXEL_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLPixelImage1D(const Texture1D<TPIXEL_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLPixelImage1D(const Texture1D<TPIXEL_TYPE>::PureColorParams &params)
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
class OpenGLPixelImage2D : public PixelImage2D<TPIXEL_TYPE>,
                           public OpenGLPixelImage2DBit,
                           public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLPixelImage2D(const Texture2D<TPIXEL_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLPixelImage2D(const Texture2D<TPIXEL_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLPixelImage2D(const Texture2D<TPIXEL_TYPE>::PureColorParams &params)
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
class OpenGLPixelImage3D : public PixelImage3D<TPIXEL_TYPE>,
                           public OpenGLPixelImage3DBit,
                           public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLPixelImage3D(const Texture3D<TPIXEL_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLPixelImage3D(const Texture3D<TPIXEL_TYPE>::EmptyParams &params)
    {
        setupEmpty({params.size, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLPixelImage3D(const Texture3D<TPIXEL_TYPE>::PureColorParams &params)
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
class OpenGLPixelImageCube : public dynasma::PtrAwareBase,
                             public PixelImageCubemap<TPIXEL_TYPE>,
                             public OpenGLPixelImageCubeBit,
                             public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLPixelImageCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                         const TextureCubemap<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLPixelImageCube(dynasma::PolymorphicReferenceCounter *p_ctr,
                         const TextureCubemap<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, params.size, 6, 0}, params.storageFormat,
                   promotePadVector(params.swizzle), params.filtering, params.friendlyName);
    }
    OpenGLPixelImageCube(dynasma::PolymorphicReferenceCounter *p_ctr,
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

    // For PixelImageCubemap

    dynasma::PinPtr<PixelImage2D<TPIXEL_TYPE>> getFace(Side side) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const PixelImage2D<TPIXEL_TYPE>> getFace(Side side) const override
    {
        /// TODO: something
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLPixelImage1DLayered : public dynasma::PtrAwareBase,
                                  public PixelImage1DLayered<TPIXEL_TYPE>,
                                  public OpenGLPixelImage1DLayeredBit,
                                  public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLPixelImage1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                              const Texture1DLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLPixelImage1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                              const Texture1DLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, 0, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLPixelImage1DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
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

    // For PixelImage1DLayered

    dynasma::PinPtr<PixelImage1D<TPIXEL_TYPE>> getLayer(std::size_t y) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const PixelImage1D<TPIXEL_TYPE>> getLayer(std::size_t y) const override
    {
        /// TODO: something
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLPixelImage2DLayered : public dynasma::PtrAwareBase,
                                  public PixelImage2DLayered<TPIXEL_TYPE>,
                                  public OpenGLPixelImage2DLayeredBit,
                                  public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLPixelImage2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                              const Texture2DLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLPixelImage2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                              const Texture2DLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size, 0}, params.storageFormat, promotePadVector(params.swizzle),
                   params.filtering, params.friendlyName);
    }
    OpenGLPixelImage2DLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
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

    // For PixelImage2DLayered

    dynasma::PinPtr<PixelImage2D<TPIXEL_TYPE>> getLayer(std::size_t z) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const PixelImage2D<TPIXEL_TYPE>> getLayer(std::size_t z) const override
    {
        /// TODO: something
    }
};

template <PixelType TPIXEL_TYPE>
class OpenGLPixelImageCubeLayered : public dynasma::PtrAwareBase,
                                    public PixelImageCubemapLayered<TPIXEL_TYPE>,
                                    public OpenGLPixelImageCubeLayeredBit,
                                    public OpenGLTensorPixelTypedBit<TPIXEL_TYPE>
{
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::getPixelFormatBit;
    using OpenGLTensorPixelTypedBit<TPIXEL_TYPE>::setPixelFormatBit;

  public:
    OpenGLPixelImageCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const TextureCubemapLayered<TPIXEL_TYPE>::FileLoadParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupFromFile(TPIXEL_TYPE, params);
    }
    OpenGLPixelImageCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
                                const TextureCubemapLayered<TPIXEL_TYPE>::EmptyParams &params)
        : dynasma::PtrAwareBase(p_ctr)
    {
        setupEmpty({params.size.x, params.size.x, 6, params.size.y}, params.storageFormat,
                   promotePadVector(params.swizzle), params.filtering, params.friendlyName);
    }
    OpenGLPixelImageCubeLayered(dynasma::PolymorphicReferenceCounter *p_ctr,
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

    // For PixelImageCubemapLayered

    dynasma::PinPtr<PixelImageCubemap<TPIXEL_TYPE>> getLayer(std::size_t z) override
    {
        /// TODO: something
    }

    dynasma::PinPtr<const PixelImageCubemap<TPIXEL_TYPE>> getLayer(std::size_t z) const override
    {
        /// TODO: something
    }
};

} // namespace Vitrae