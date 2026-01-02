#pragma once

#include "Vitrae/Assets/TensorBuffer.hpp"
#include "Vitrae/Data/BufferFormat.hpp"
#include "Vitrae/Setup/ImageLoad.hpp"
#include "Vitrae/Setup/TextureFiltering.hpp"
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

// ==== TensorBuffer dimensioned bits
// ===================================================================

class OpenGLTensorBuffer1DBit : public OpenGLTensorBufferBit
{
  public:
    unsigned int getSize() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLTensorBuffer2DBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec2 getSize() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec2 m_size;
};

class OpenGLTensorBuffer3DBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec3 getSize() const { return m_size; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    glm::uvec3 m_size;
};

class OpenGLTensorBufferCubeBit : public OpenGLTensorBufferBit
{
  public:
    glm::uvec3 getSize() const { return {m_size, m_size, 6}; }

  protected:
    void setSize(int width, int height, int count) override;
    void setSize(glm::uvec4 ndSize) override;
    void loadDataToGPU(const GpuFormatInfo &storage, std::span<const LoadInfo> load) override;

    unsigned int m_size;
};

class OpenGLTensorBuffer1DLayeredBit : public OpenGLTensorBufferBit
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

class OpenGLTensorBuffer2DLayeredBit : public OpenGLTensorBufferBit
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

class OpenGLTensorBufferCubeLayeredBit : public OpenGLTensorBufferBit
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

// ==== TensorBuffer mix-ins
// ============================================================================

template <BufferType TBUFFER_TYPE>
class OpenGLTensorBuffer1D : public TensorBuffer1D<TBUFFER_TYPE>, public OpenGLTensorBuffer1DBit
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
class OpenGLTensorBuffer2D : public TensorBuffer2D<TBUFFER_TYPE>, public OpenGLTensorBuffer2DBit
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
class OpenGLTensorBuffer3D : public TensorBuffer3D<TBUFFER_TYPE>, public OpenGLTensorBuffer3DBit
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
class OpenGLTensorBufferCube : public TensorBufferCubemap<TBUFFER_TYPE>,
                               public OpenGLTensorBufferCubeBit
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
                                    public OpenGLTensorBuffer1DLayeredBit
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
                                    public OpenGLTensorBuffer2DLayeredBit
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
                                      public OpenGLTensorBufferCubeLayeredBit
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