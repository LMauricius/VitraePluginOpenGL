#pragma once

#include "Vitrae/Assets/Texture.hpp"
#include "VitraePluginOpenGL/Specializations/TensorBuffer.hpp"
#include "glad/glad.h"

namespace Vitrae
{

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

// ==== TensorBuffer mix-ins =======================================================================

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