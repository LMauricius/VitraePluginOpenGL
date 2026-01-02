#pragma once

#include "Vitrae/Assets/Texture.hpp"
#include "Vitrae/Data/BufferFormat.hpp"
#include "Vitrae/Setup/ImageLoad.hpp"
#include "Vitrae/Setup/TextureFiltering.hpp"
#include "glad/glad.h"

#include <filesystem>

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
 * A Texture is a single image-like resource
 */
class OpenGLTextureBit
{
  public:
    OpenGLTextureBit();
    virtual ~OpenGLTextureBit();

    std::size_t memory_cost() const;

    GLuint glTextureId;

  protected:
    bool m_sentToGPU;

    void setupFromFile(BufferType bufferType, const ImageCommon::FileLoadParams &params);
    void setupFromColor(AnyBufferFormat bufferFormat, const void *pixelData);
    void setupEmpty(const glm::uvec4 &ndSize, AnyBufferFormat bufferFormat,
                    const glm::vec<4, ChannelSource> &swizzleSpec,
                    const TextureFilteringParams &filtering, String friendlyName);

    // void readFile(const ImageCommon::FileLoadParams &params);
    // void createTexture();
    // void loadFiltering(const GpuFormatInfo &formatInfo, const TextureFilteringParams &filtering);
    virtual void setSize(int width, int height, int count) = 0;
    virtual void setSize(glm::uvec4 ndSize) = 0;
    virtual void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) = 0;

  private:
};

// ==== Texture dimensioned bits ===================================================================

class OpenGLTexture1DBit : public OpenGLTextureBit
{
  public:
    unsigned int getSize() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLTexture2DBit : public OpenGLTextureBit
{
  public:
    glm::uvec2 getSize() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec2 m_size;
};

class OpenGLTexture3DBit : public OpenGLTextureBit
{
  public:
    glm::uvec3 getSize() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec3 m_size;
};

class OpenGLTextureCubeBit : public OpenGLTextureBit
{
  public:
    glm::uvec3 getSize() const { return {m_size, m_size, 6}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLTexture1DLayeredBit : public OpenGLTextureBit
{
  public:
    glm::uvec2 getSize() const { return {m_size, m_layerCount}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
    unsigned int m_layerCount;
};

class OpenGLTexture2DLayeredBit : public OpenGLTextureBit
{
  public:
    glm::uvec3 getSize() const { return {m_size, m_layerCount}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec2 m_size;
    unsigned int m_layerCount;
};

class OpenGLTextureCubeLayeredBit : public OpenGLTextureBit
{
  public:
    glm::uvec4 getSize() const { return {m_size, m_size, 6, m_layerCount}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
    unsigned int m_layerCount;
};

// ==== Texture mix-ins ============================================================================

template <BufferType TBUFFER_TYPE>
class OpenGLTexture1D : public Texture1D<TBUFFER_TYPE>, public OpenGLTexture1DBit
{
  public:
    OpenGLTexture1D(const Texture1D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTexture1D(const Texture1D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTexture1D(const Texture1D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};
template <BufferType TBUFFER_TYPE>
class OpenGLTexture2D : public Texture2D<TBUFFER_TYPE>, public OpenGLTexture2DBit
{
  public:
    OpenGLTexture2D(const Texture2D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTexture2D(const Texture2D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTexture2D(const Texture2D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture3D : public Texture3D<TBUFFER_TYPE>, public OpenGLTexture3DBit
{
  public:
    OpenGLTexture3D(const Texture3D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTexture3D(const Texture3D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTexture3D(const Texture3D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTextureCube : public TextureCubemap<TBUFFER_TYPE>, public OpenGLTextureCubeBit
{
  public:
    OpenGLTextureCube(const TextureCubemap<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTextureCube(const TextureCubemap<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTextureCube(const TextureCubemap<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture1DLayered : public Texture1DLayered<TBUFFER_TYPE>,
                               public OpenGLTexture1DLayeredBit
{
  public:
    OpenGLTexture1DLayered(const Texture1DLayered<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTexture1DLayered(const Texture1DLayered<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTexture1DLayered(const Texture1DLayered<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTexture2DLayered : public Texture2DLayered<TBUFFER_TYPE>,
                               public OpenGLTexture2DLayeredBit
{
  public:
    OpenGLTexture2DLayered(const Texture2DLayered<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTexture2DLayered(const Texture2DLayered<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTexture2DLayered(const Texture2DLayered<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTextureCubeLayered : public TextureCubemapLayered<TBUFFER_TYPE>,
                                 public OpenGLTextureCubeLayeredBit
{
  public:
    OpenGLTextureCubeLayered(const TextureCubemapLayered<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTextureCubeLayered(const TextureCubemapLayered<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTextureCubeLayered(const TextureCubemapLayered<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

// ==== TensorBuffer mix-ins
// ============================================================================

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer1D : public TensorBuffer1D<TBUFFER_TYPE>, public OpenGLTexture1DBit
{
  public:
    OpenGLTensorBuffer1D(const TensorBuffer1D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer1D(const TensorBuffer1D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTensorBuffer1D(const TensorBuffer1D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};
template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer2D : public TensorBuffer2D<TBUFFER_TYPE>, public OpenGLTexture2DBit
{
  public:
    OpenGLTensorBuffer2D(const TensorBuffer2D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer2D(const TensorBuffer2D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTensorBuffer2D(const TensorBuffer2D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer3D : public TensorBuffer3D<TBUFFER_TYPE>, public OpenGLTexture3DBit
{
  public:
    OpenGLTensorBuffer3D(const TensorBuffer3D<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer3D(const TensorBuffer3D<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTensorBuffer3D(const TensorBuffer3D<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBufferCube : public TensorBufferCubemap<TBUFFER_TYPE>, public OpenGLTextureCubeBit
{
  public:
    OpenGLTensorBufferCube(const TensorBufferCubemap<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBufferCube(const TensorBufferCubemap<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTensorBufferCube(const TensorBufferCubemap<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer1DLayered : public TensorBuffer1DLayered<TBUFFER_TYPE>,
                                    public OpenGLTexture1DLayeredBit
{
  public:
    OpenGLTensorBuffer1DLayered(const TensorBuffer1DLayered<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer1DLayered(const TensorBuffer1DLayered<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTensorBuffer1DLayered(const TensorBuffer1DLayered<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer2DLayered : public TensorBuffer2DLayered<TBUFFER_TYPE>,
                                    public OpenGLTexture2DLayeredBit
{
  public:
    OpenGLTensorBuffer2DLayered(const TensorBuffer2DLayered<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBuffer2DLayered(const TensorBuffer2DLayered<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTensorBuffer2DLayered(const TensorBuffer2DLayered<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBufferCubeLayered : public TensorBufferCubemapLayered<TBUFFER_TYPE>,
                                      public OpenGLTextureCubeLayeredBit
{
  public:
    OpenGLTensorBufferCubeLayered(
        const TensorBufferCubemapLayered<TBUFFER_TYPE>::FileLoadParams &params)
    {
        setupFromFile(TBUFFER_TYPE, params);
    }
    OpenGLTensorBufferCubeLayered(
        const TensorBufferCubemapLayered<TBUFFER_TYPE>::EmptyParams &params)
    {
        setupEmpty(params.ndSize, params.storageFormat, params.swizzleSpec, params.filtering,
                   params.friendlyName);
    }
    OpenGLTensorBufferCubeLayered(
        const TensorBufferCubemapLayered<TBUFFER_TYPE>::PureColorParams &params)
    {
        setupFromColor(params.storageFormat, &params.color);
    }
};

} // namespace Vitrae