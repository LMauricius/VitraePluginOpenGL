#include "VitraePluginOpenGL/Specializations/TensorBuffer.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Data/BufferFormat.hpp"
#include "Vitrae/Setup/TextureFiltering.hpp"
#include "VitraePluginOpenGL/Specializations/Renderer.hpp"

#include "stb/stb_image.h"

#include <map>
#include <vector>

namespace Vitrae
{
// ==== Helpers ====================================================================================

namespace
{

GLint BUFFER_TYPE_2_GL_CPU_FORMAT[] = {
    /*       [REAL_SCALAR] = */ GL_RED,
    /*         [REAL_VEC2] = */ GL_RG,
    /*         [REAL_VEC3] = */ GL_RGB,
    /*         [REAL_VEC4] = */ GL_RGBA,
    /*      [WHOLE_SCALAR] = */ GL_RED,
    /*        [WHOLE_VEC2] = */ GL_RG,
    /*        [WHOLE_VEC3] = */ GL_RGB,
    /*        [WHOLE_VEC4] = */ GL_RGBA,
    /*      [COUNT_SCALAR] = */ GL_RED,
    /*        [COUNT_VEC2] = */ GL_RG,
    /*        [COUNT_VEC3] = */ GL_RGB,
    /*        [COUNT_VEC4] = */ GL_RGBA,
    /*             [DEPTH] = */ GL_DEPTH_COMPONENT,
    /*           [STENCIL] = */ GL_STENCIL_INDEX,
    /* [DEPTH_AND_STENCIL] = */ GL_DEPTH_STENCIL,
};

GLint BUFFER_TYPE_2_GL_CPU_TYPE[] = {
    /*       [REAL_SCALAR] = */ GL_FLOAT,
    /*         [REAL_VEC2] = */ GL_FLOAT,
    /*         [REAL_VEC3] = */ GL_FLOAT,
    /*         [REAL_VEC4] = */ GL_FLOAT,
    /*      [WHOLE_SCALAR] = */ GL_INT,
    /*        [WHOLE_VEC2] = */ GL_INT,
    /*        [WHOLE_VEC3] = */ GL_INT,
    /*        [WHOLE_VEC4] = */ GL_INT,
    /*      [COUNT_SCALAR] = */ GL_UNSIGNED_INT,
    /*        [COUNT_VEC2] = */ GL_UNSIGNED_INT,
    /*        [COUNT_VEC3] = */ GL_UNSIGNED_INT,
    /*        [COUNT_VEC4] = */ GL_UNSIGNED_INT,
    /*             [DEPTH] = */ GL_FLOAT,
    /*           [STENCIL] = */ GL_UNSIGNED_INT,
    /* [DEPTH_AND_STENCIL] = */ GL_UNSIGNED_BYTE,
};

GLint REAL_SCALAR_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC_FULL] = */ GL_R32F,
    /* [GENERIC_NORM] = */ GL_RED,
    /* [LINEAR_COLOR] = */ GL_RED,
    /*      [FLOAT16] = */ GL_R16F,
    /*      [FLOAT32] = */ GL_R32F,
    /*        [NORM8] = */ GL_R8,
    /*       [NORM16] = */ GL_R16,
    /*       [SNORM8] = */ GL_R8_SNORM,
    /*      [SNORM16] = */ GL_R16_SNORM,
};

GLint REAL_VEC2_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC_FULL] = */ GL_RG32F,
    /* [GENERIC_NORM] = */ GL_RG,
    /* [LINEAR_COLOR] = */ GL_RG,
    /*      [FLOAT16] = */ GL_RG16F,
    /*      [FLOAT32] = */ GL_RG32F,
    /*        [NORM8] = */ GL_RG8,
    /*       [NORM16] = */ GL_RG16,
    /*       [SNORM8] = */ GL_RG8_SNORM,
    /*      [SNORM16] = */ GL_RG16_SNORM,
};

GLint REAL_VEC3_2_GL_INTERNAL_FORMAT[] = {
    /*    [GENERIC_FULL] = */ GL_RGB32F,
    /*    [GENERIC_NORM] = */ GL_RGB,
    /*    [LINEAR_COLOR] = */ GL_RGB,
    /*      [SRGB_COLOR] = */ GL_SRGB8,
    /*         [FLOAT16] = */ GL_RGB16F,
    /*         [FLOAT32] = */ GL_RGB32F,
    /*           [NORM4] = */ GL_RGB4,
    /*           [NORM5] = */ GL_RGB5,
    /*           [NORM8] = */ GL_RGB8,
    /*          [NORM10] = */ GL_RGB10,
    /*          [NORM12] = */ GL_RGB12,
    /*          [NORM16] = */ GL_RGB16,
    /*          [SNORM8] = */ GL_RGB8_SNORM,
    /*         [SNORM16] = */ GL_RGB16_SNORM,

    /*      [NORM_3_3_2] = */ GL_R3_G3_B2,
    /* [UFLOAT_11_11_10] = */ GL_R11F_G11F_B10F,
    /*    [UFLOAT9_EXP5] = */ GL_RGB9_E5,
};

GLint REAL_VEC4_2_GL_INTERNAL_FORMAT[] = {
    /*     [GENERIC_FULL] = */ GL_RGBA32F,
    /*     [GENERIC_NORM] = */ GL_RGBA,
    /*     [LINEAR_COLOR] = */ GL_RGBA,
    /* [SRGB_ALPHA_COLOR] = */ GL_SRGB8_ALPHA8,
    /*          [FLOAT16] = */ GL_RGBA16F,
    /*          [FLOAT32] = */ GL_RGBA32F,
    /*            [NORM2] = */ GL_RGBA2,
    /*            [NORM4] = */ GL_RGBA4,
    /*            [NORM8] = */ GL_RGBA8,
    /*           [NORM12] = */ GL_RGBA12,
    /*           [NORM16] = */ GL_RGBA16,
    /*           [SNORM8] = */ GL_RGBA8_SNORM,
    /*          [SNORM16] = */ GL_RGBA16_SNORM,

    /*     [NORM_5_5_5_1] = */ GL_RGB5_A1,
    /*  [NORM_10_10_10_2] = */ GL_RGB10_A2,
};

GLint WHOLE_SCALAR_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_R32I,
    /*    [INT8] = */ GL_R8I,
    /*   [INT16] = */ GL_R16I,
    /*   [INT32] = */ GL_R32I,
};

GLint WHOLE_VEC2_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_RG32I,
    /*    [INT8] = */ GL_RG8I,
    /*   [INT16] = */ GL_RG16I,
    /*   [INT32] = */ GL_RG32I,
};

GLint WHOLE_VEC3_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_RGB32I,
    /*    [INT8] = */ GL_RGB8I,
    /*   [INT16] = */ GL_RGB16I,
    /*   [INT32] = */ GL_RGB32I,
};

GLint WHOLE_VEC4_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_RGBA32I,
    /*    [INT8] = */ GL_RGBA8I,
    /*   [INT16] = */ GL_RGBA16I,
    /*   [INT32] = */ GL_RGBA32I,
};

GLint COUNT_SCALAR_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_R32UI,
    /*    [INT8] = */ GL_R8UI,
    /*   [INT16] = */ GL_R16UI,
    /*   [INT32] = */ GL_R32UI,
};

GLint COUNT_VEC2_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_RG32UI,
    /*    [INT8] = */ GL_RG8UI,
    /*   [INT16] = */ GL_RG16UI,
    /*   [INT32] = */ GL_RG32UI,
};

GLint COUNT_VEC3_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_RGB32UI,
    /*    [INT8] = */ GL_RGB8UI,
    /*   [INT16] = */ GL_RGB16UI,
    /*   [INT32] = */ GL_RGB32UI,
};

GLint COUNT_VEC4_2_GL_INTERNAL_FORMAT[] = {
    /*             [GENERIC] = */ GL_RGBA32UI,
    /*                [INT8] = */ GL_RGBA8UI,
    /*               [INT16] = */ GL_RGBA16UI,
    /*               [INT32] = */ GL_RGBA32UI,
    /* [UNSIGNED_10_10_10_2] = */ GL_RGB10_A2UI,
};

GLint DEPTH_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_DEPTH_COMPONENT,
    /*  [NORM16] = */ GL_DEPTH_COMPONENT16,
    /*  [NORM24] = */ GL_DEPTH_COMPONENT24,
    /*  [NORM32] = */ GL_DEPTH_COMPONENT32,
    /* [FLOAT32] = */ GL_DEPTH_COMPONENT32F,
};

GLint STENCIL_2_GL_INTERNAL_FORMAT[] = {
    /* [GENERIC] = */ GL_STENCIL_INDEX,
    /* [NORM8] = */ GL_STENCIL_INDEX8,
    /* [NORM16] = */ GL_STENCIL_INDEX16,
};

GLint DEPTH_AND_STENCIL_2_GL_INTERNAL_FORMAT[] = {
    /*           [GENERIC] = */ GL_DEPTH24_STENCIL8,
    /*  [NORM24_UNSIGNED8] = */ GL_DEPTH24_STENCIL8,
    /* [FLOAT32_UNSIGNED8] = */ GL_DEPTH32F_STENCIL8,
};

GLint *BUFFER_FORMAT_2_GL_INTERNAL_FORMAT[] = {
    /*       [REAL_SCALAR] = */ REAL_SCALAR_2_GL_INTERNAL_FORMAT,
    /*         [REAL_VEC2] = */ REAL_VEC2_2_GL_INTERNAL_FORMAT,
    /*         [REAL_VEC3] = */ REAL_VEC3_2_GL_INTERNAL_FORMAT,
    /*         [REAL_VEC4] = */ REAL_VEC4_2_GL_INTERNAL_FORMAT,
    /*      [WHOLE_SCALAR] = */ WHOLE_SCALAR_2_GL_INTERNAL_FORMAT,
    /*        [WHOLE_VEC2] = */ WHOLE_VEC2_2_GL_INTERNAL_FORMAT,
    /*        [WHOLE_VEC3] = */ WHOLE_VEC3_2_GL_INTERNAL_FORMAT,
    /*        [WHOLE_VEC4] = */ WHOLE_VEC4_2_GL_INTERNAL_FORMAT,
    /*      [COUNT_SCALAR] = */ COUNT_SCALAR_2_GL_INTERNAL_FORMAT,
    /*        [COUNT_VEC2] = */ COUNT_VEC2_2_GL_INTERNAL_FORMAT,
    /*        [COUNT_VEC3] = */ COUNT_VEC3_2_GL_INTERNAL_FORMAT,
    /*        [COUNT_VEC4] = */ COUNT_VEC4_2_GL_INTERNAL_FORMAT,
    /*             [DEPTH] = */ DEPTH_2_GL_INTERNAL_FORMAT,
    /*           [STENCIL] = */ STENCIL_2_GL_INTERNAL_FORMAT,
    /* [DEPTH_AND_STENCIL] = */ DEPTH_AND_STENCIL_2_GL_INTERNAL_FORMAT,
};

GLint getInternalFormat(AnyBufferFormat bufferFormat)
{
    // BufferType corresponds to the active index of the variant
    std::size_t bufferTypeIndex = bufferFormat.index();
    // BufferFormat retrieval *should* get optimized into just getting an integer since the same
    // code runs for each option
    std::size_t bufferFormatIndex = std::visit(
        [&](auto &&bufferType) { return static_cast<std::size_t>(bufferType); }, bufferFormat);

    return BUFFER_FORMAT_2_GL_INTERNAL_FORMAT[bufferTypeIndex][bufferFormatIndex];
}
LoadInfo readImageFile(ComponentRoot &root, std::filesystem::path filepath, int &w, int &h,
                       int &stbChannelFormat)
{
    LoadInfo ret;
    std::size_t bitsPerComponent, numComponents;

    if (stbi_is_hdr(filepath.c_str())) {
        ret.pixelData = stbi_loadf(filepath.c_str(), &w, &h, &stbChannelFormat, STBI_default);
        ret.metaData.type = GL_FLOAT;
        bitsPerComponent = 32;
    } else if (stbi_is_16_bit(filepath.c_str())) {
        ret.pixelData = stbi_load_16(filepath.c_str(), &w, &h, &stbChannelFormat, STBI_default);
        ret.metaData.type = GL_UNSIGNED_SHORT;
        bitsPerComponent = 32;
    } else {
        ret.pixelData = stbi_load(filepath.c_str(), &w, &h, &stbChannelFormat, STBI_default);
        ret.metaData.type = GL_UNSIGNED_BYTE;
        bitsPerComponent = 32;
    }

    if (stbi_failure_reason()) {
        root.getErrStream() << "Texture load from '" << filepath
                            << "' failed: " << stbi_failure_reason() << std::endl;
        stbi_image_free(ret.pixelData);
        ret.pixelData = nullptr;
    } else {
        root.getInfoStream() << "Texture '" << filepath << "' loaded." << std::endl;
    }

    switch (stbChannelFormat) {
    case STBI_grey:
        ret.metaData.format = GL_RED;
        numComponents = 1;
        break;
    case STBI_grey_alpha:
        ret.metaData.format = GL_RG;
        numComponents = 2;
        break;
    case STBI_rgb:
        ret.metaData.format = GL_RGB;
        numComponents = 3;
        break;
    case STBI_rgb_alpha:
        ret.metaData.format = GL_RGBA;
        numComponents = 4;
        break;
    default:
        numComponents = 0;
        break;
    }

    ret.metaData.bitsPerElement = bitsPerComponent * numComponents;
    return ret;
}

GpuFormatInfo getGpuFormatInfo(BufferType internalType, int stbChannelFormat, GLenum dataType)
{
    GpuFormatInfo ret;

    switch (internalType) {
    case BufferType::REAL_SCALAR:
        ret.useSwizzle = false;

        switch (dataType) {
        case GL_FLOAT:
            ret.internalFormat = GL_R32F;
            break;
        case GL_UNSIGNED_SHORT:
            ret.internalFormat = GL_R16;
            break;
        case GL_UNSIGNED_BYTE:
            ret.internalFormat = GL_RED;
            break;
        default:
            ret.internalFormat = GL_INVALID_ENUM;
            break;
        }
        break;
    case BufferType::REAL_VEC2:
        switch (stbChannelFormat) {
        case STBI_grey:
            ret.swizzleVal = {GL_RED, GL_RED, GL_ZERO, GL_ZERO};
            ret.useSwizzle = true;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_R32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_R16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RED;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
        case STBI_grey_alpha:
        case STBI_rgb:
        case STBI_rgb_alpha:
            ret.useSwizzle = false;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_RG32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_RG16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RG;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
        }
        break;
    case BufferType::REAL_VEC3:
        switch (stbChannelFormat) {
        case STBI_grey:
        case STBI_grey_alpha:
            ret.swizzleVal = {GL_RED, GL_RED, GL_RED, GL_ZERO};
            ret.useSwizzle = true;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_R32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_R16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RED;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
        case STBI_rgb:
        case STBI_rgb_alpha:
            ret.useSwizzle = false;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_RGB32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_RGB16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RGB;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
            break;
        default:
            break;
        }

        break;
    case BufferType::REAL_VEC4:
        switch (stbChannelFormat) {
        case STBI_grey:
            ret.swizzleVal = {GL_RED, GL_RED, GL_RED, GL_ONE};
            ret.useSwizzle = true;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_R32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_R16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RED;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
            break;
        case STBI_grey_alpha:
            ret.swizzleVal = {GL_RED, GL_RED, GL_RED, GL_GREEN};
            ret.useSwizzle = true;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_RG32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_RG16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RG;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
            break;
        case STBI_rgb:
            ret.swizzleVal = {GL_RED, GL_GREEN, GL_BLUE, GL_ONE};
            ret.useSwizzle = true;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_RGB32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_RGB16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RGB;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
            break;
        case STBI_rgb_alpha:
            ret.useSwizzle = false;

            switch (dataType) {
            case GL_FLOAT:
                ret.internalFormat = GL_RGBA32F;
                break;
            case GL_UNSIGNED_SHORT:
                ret.internalFormat = GL_RGBA16;
                break;
            case GL_UNSIGNED_BYTE:
                ret.internalFormat = GL_RGBA;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
            break;
        default:
            break;
        }
        break;
    default: {
        // BufferType corresponds to the active index of the variant
        std::size_t bufferTypeIndex = (std::size_t)internalType;

        // Use the default/first format per buffer type, should be good enough
        std::size_t bufferFormatIndex = 0;

        ret.internalFormat = BUFFER_FORMAT_2_GL_INTERNAL_FORMAT[bufferTypeIndex][bufferFormatIndex];
        break;
    }
    }

    return ret;
}

GpuFormatInfo getGpuFormatInfo(AnyBufferFormat bufferFormat, glm::vec<4, ChannelSource> swizzleSpec)
{
    GpuFormatInfo ret;

    ret.internalFormat = getInternalFormat(bufferFormat);

    BufferType bufferType = static_cast<BufferType>(bufferFormat.index());

    ret.swizzleVal = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA};

    for (int i = 0; i < getChannelCount(bufferType); ++i) {
        switch (swizzleSpec[i]) {
        case ChannelSource::SLOT_0:
            ret.swizzleArr[i] = GL_RED;
            break;
        case ChannelSource::SLOT_1:
            ret.swizzleArr[i] = GL_GREEN;
            break;
        case ChannelSource::SLOT_2:
            ret.swizzleArr[i] = GL_BLUE;
            break;
        case ChannelSource::SLOT_3:
            ret.swizzleArr[i] = GL_ALPHA;
            break;
        case ChannelSource::CONSTANT_0:
            ret.swizzleArr[i] = GL_ZERO;
            break;
        case ChannelSource::CONSTANT_1:
            ret.swizzleArr[i] = GL_ONE;
            break;
        }
    }

    ret.useSwizzle = (ret.swizzleVal.r != GL_RED || ret.swizzleVal.g != GL_GREEN ||
                      ret.swizzleVal.b != GL_BLUE || ret.swizzleVal.a != GL_ALPHA);

    return ret;
}

CpuDataInfo getNativeCpuDataInfo(BufferType bufferType)
{
    CpuDataInfo ret;
    std::size_t componentBits, componentCount;

    switch (bufferType) {
    case BufferType::REAL_SCALAR:
    case BufferType::REAL_VEC2:
    case BufferType::REAL_VEC3:
    case BufferType::REAL_VEC4:
        ret.type = GL_FLOAT;
        componentBits = sizeof(GLfloat) * 8;
        break;
    case BufferType::WHOLE_SCALAR:
    case BufferType::WHOLE_VEC2:
    case BufferType::WHOLE_VEC3:
    case BufferType::WHOLE_VEC4:
        ret.type = GL_INT;
        componentBits = sizeof(GLint) * 8;
        break;
    case BufferType::COUNT_SCALAR:
    case BufferType::COUNT_VEC2:
    case BufferType::COUNT_VEC3:
    case BufferType::COUNT_VEC4:
        ret.type = GL_UNSIGNED_INT;
        componentBits = sizeof(GLuint) * 8;
        break;
    case BufferType::DEPTH:
        ret.type = GL_FLOAT;
        componentBits = sizeof(GLfloat) * 8;
        break;
    case BufferType::STENCIL:
        ret.type = GL_UNSIGNED_INT;
        componentBits = sizeof(GLuint) * 8;
        break;
    case BufferType::DEPTH_AND_STENCIL:
        ret.type = GL_INVALID_ENUM;
        componentBits = 0;
        break;
    }

    switch (bufferType) {
    case BufferType::REAL_SCALAR:
    case BufferType::WHOLE_SCALAR:
    case BufferType::COUNT_SCALAR:
    case BufferType::DEPTH:
    case BufferType::STENCIL:
        ret.format = GL_RED;
        componentCount = 1;
        break;
    case BufferType::REAL_VEC2:
    case BufferType::WHOLE_VEC2:
    case BufferType::COUNT_VEC2:
        ret.format = GL_RG;
        componentCount = 2;
        break;
    case BufferType::REAL_VEC3:
    case BufferType::WHOLE_VEC3:
    case BufferType::COUNT_VEC3:
        ret.format = GL_RGB;
        componentCount = 3;
        break;
    case BufferType::REAL_VEC4:
    case BufferType::WHOLE_VEC4:
    case BufferType::COUNT_VEC4:
        ret.format = GL_RGBA;
        componentCount = 4;
        break;
    case BufferType::DEPTH_AND_STENCIL:
        ret.format = GL_DEPTH_STENCIL;
        componentCount = 2;
        break;
    }

    ret.bitsPerElement = componentBits * componentCount;

    return ret;
}

void loadFiltering(GLint glTextureId, const GpuFormatInfo &formatInfo,
                   const TextureFilteringParams &filtering)
{
    GLint glMagFilter, glMinFilter, glWrapS, glWrapT;
    bool mUseMipMaps;
    glm::vec4 borderColor;

    if (std::holds_alternative<WrappingType>(filtering.horWrap)) {
        switch (std::get<WrappingType>(filtering.horWrap)) {
        case WrappingType::BORDER_COLOR:
            glWrapS = GL_CLAMP_TO_BORDER;
            break;
        case WrappingType::CLAMP:
            glWrapS = GL_CLAMP_TO_EDGE;
            break;
        case WrappingType::REPEAT:
            glWrapS = GL_REPEAT;
            break;
        case WrappingType::MIRROR:
            glWrapS = GL_MIRRORED_REPEAT;
            break;
        }
    } else {
        glWrapS = GL_REPEAT;
    }

    if (std::holds_alternative<WrappingType>(filtering.verWrap)) {
        switch (std::get<WrappingType>(filtering.verWrap)) {
        case WrappingType::BORDER_COLOR:
            glWrapT = GL_CLAMP_TO_BORDER;
            break;
        case WrappingType::CLAMP:
            glWrapT = GL_CLAMP_TO_EDGE;
            break;
        case WrappingType::REPEAT:
            glWrapT = GL_REPEAT;
            break;
        case WrappingType::MIRROR:
            glWrapT = GL_MIRRORED_REPEAT;
            break;
        }
    } else {
        glWrapT = GL_REPEAT;
    }

    if (std::holds_alternative<FilterType>(filtering.magFilter)) {
        switch (std::get<FilterType>(filtering.magFilter)) {
        case FilterType::LINEAR:
            glMagFilter = GL_LINEAR;
            break;
        case FilterType::NEAREST:
            glMagFilter = GL_NEAREST;
            break;
        }
    } else {
        glMagFilter = GL_NEAREST;
    }

    if (std::holds_alternative<bool>(filtering.useMipMaps)) {
        mUseMipMaps = std::get<bool>(filtering.useMipMaps);

        if (std::holds_alternative<FilterType>(filtering.minFilter)) {
            switch (std::get<FilterType>(filtering.minFilter)) {
            case FilterType::LINEAR:
                glMinFilter = GL_LINEAR;
                break;
            case FilterType::NEAREST:
                glMinFilter = GL_NEAREST;
                break;
            }
        } else {
            glMinFilter = GL_NEAREST;
        }
    } else {
        mUseMipMaps = false;
        glMinFilter = GL_NEAREST;
    }

    if (std::holds_alternative<glm::vec4>(filtering.borderColor)) {
        borderColor = std::get<glm::vec4>(filtering.borderColor);
    }

    if (formatInfo.useSwizzle) {
        glTextureParameteriv(glTextureId, GL_TEXTURE_SWIZZLE_RGBA, formatInfo.swizzleArr);
    }

    glTextureParameteri(glTextureId, GL_TEXTURE_MAG_FILTER, glMagFilter);
    glTextureParameteri(glTextureId, GL_TEXTURE_MIN_FILTER, glMinFilter);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_S, glWrapS);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_T, glWrapT);
    glTextureParameterfv(glTextureId, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
}

} // namespace

OpenGLTensorBufferBit::OpenGLTensorBufferBit() {}

OpenGLTensorBufferBit::~OpenGLTensorBufferBit()
{
    glDeleteTextures(1, &glTextureId);
}

std::size_t OpenGLTensorBufferBit::gpuMemoryCostBit() const
{
    return 0;
}

void OpenGLTensorBufferBit::setupFromFile(BufferType bufferType,
                                          const ImageCommon::FileLoadParams &params)
{
    m_sentToGPU = true;
    glGenTextures(1, &glTextureId);

    int stbChannelFormat, width, height;
    LoadInfo load[] = {
        readImageFile(params.root, params.filepath, width, height, stbChannelFormat),
    };

    GpuFormatInfo storageInfo =
        getGpuFormatInfo(bufferType, stbChannelFormat, load[0].metaData.type);

    loadFiltering(glTextureId, storageInfo, params.filtering);
    setSize(width, height, 1);
    loadDataToGPU(storageInfo, load);

    stbi_image_free(load[0].pixelData);

    if (std::holds_alternative<bool>(params.filtering.useMipMaps) &&
        std::get<bool>(params.filtering.useMipMaps)) {
        glGenerateTextureMipmap(glTextureId);
    }

    String glLabel = String("texture ") + params.filepath.c_str();
    glBindTexture(GL_TEXTURE_2D, glTextureId);
    glObjectLabel(GL_TEXTURE, glTextureId, glLabel.size(), glLabel.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTensorBufferBit::setupFromColor(AnyBufferFormat bufferFormat, const void *pixelData)
{
    m_sentToGPU = true;
    glGenTextures(1, &glTextureId);

    BufferType bufferType = static_cast<BufferType>(bufferFormat.index());

    GpuFormatInfo storageInfo = {
        .useSwizzle = false,

        // Use default (first) format for this bufferType
        .internalFormat = BUFFER_FORMAT_2_GL_INTERNAL_FORMAT[(int)bufferType][0],
    };
    LoadInfo load[] = {
        {
            .metaData = getNativeCpuDataInfo(bufferType),
            .constPixelData = pixelData,
        },
    };

    glTextureParameteri(glTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(glTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    setSize(1, 1, 1);
    loadDataToGPU(storageInfo, load);

    /// TODO: color name
    String glLabel = String("color ") + std::to_string((std::size_t)this);
    glBindTexture(GL_TEXTURE_2D, glTextureId);
    glObjectLabel(GL_TEXTURE, glTextureId, glLabel.size(), glLabel.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTensorBufferBit::setupEmpty(const glm::uvec4 &ndSize, AnyBufferFormat bufferFormat,
                                       const glm::vec<4, ChannelSource> &swizzleSpec,
                                       const TextureFilteringParams &filtering, String friendlyName)
{
    m_sentToGPU = false;
    glGenTextures(1, &glTextureId);

    GpuFormatInfo storageInfo = getGpuFormatInfo(bufferFormat, swizzleSpec);

    loadFiltering(glTextureId, storageInfo, filtering);
    setSize(ndSize);
    loadDataToGPU(storageInfo, {});

    String glLabel = String("empty ") + friendlyName;
    glBindTexture(GL_TEXTURE_2D, glTextureId);
    glObjectLabel(GL_TEXTURE, glTextureId, glLabel.size(), glLabel.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ==== Texture dimension bits =====================================================================

// ---- 1D -----------------------------------------------------------------------------------------

void OpenGLTensorBuffer1DBit::setSize(int width, int height, int count)
{
    m_size = width;
    if (height != 1) {
        throw std::runtime_error("1D texture can have only height == 1");
    }
    if (count != 1) {
        throw std::runtime_error("1D texture can have only 1 layer");
    }
}

void OpenGLTensorBuffer1DBit::loadDataToGPU(const GpuFormatInfo &storage,
                                            std::span<const LoadInfo> load)
{
    if (load.size() != 1) {
        throw std::runtime_error(
            "OpenGLTensorBuffer1DBit::loadDataToGPU: number of buffers must be 1");
    }
    glTextureStorage1D(glTextureId, 1, storage.internalFormat, m_size);
    glTextureSubImage1D(glTextureId, 0, 0, m_size, load[0].metaData.format, load[0].metaData.type,
                        load[0].pixelData);
}

// ---- 2D -----------------------------------------------------------------------------------------

void OpenGLTensorBuffer2DBit::setSize(int width, int height, int count)
{
    m_size = glm::uvec2(width, height);
    if (count != 1) {
        throw std::runtime_error("2D texture can have only 1 layer");
    }
}

void OpenGLTensorBuffer2DBit::loadDataToGPU(const GpuFormatInfo &storage,
                                            std::span<const LoadInfo> load)
{
    if (load.size() != 1) {
        throw std::runtime_error(
            "OpenGLTensorBuffer2DBit::loadDataToGPU: number of buffers must be 1");
    }
    glTextureStorage2D(glTextureId, 1, storage.internalFormat, m_size.x, m_size.y);
    glTextureSubImage2D(glTextureId, 0, 0, 0, m_size.x, m_size.y, load[0].metaData.format,
                        load[0].metaData.type, load[0].pixelData);
}

// ---- 3D -----------------------------------------------------------------------------------------

void OpenGLTensorBuffer3DBit::setSize(int width, int height, int count)
{
    m_size = glm::uvec3(width, height, 1);
}

void OpenGLTensorBuffer3DBit::loadDataToGPU(const GpuFormatInfo &storage,
                                            std::span<const LoadInfo> load)
{
    if (load.size() != 1) {
        throw std::runtime_error(
            "OpenGLTensorBuffer3DBit::loadDataToGPU: number of buffers must be 1");
    }
    glTextureStorage3D(glTextureId, 1, storage.internalFormat, m_size.x, m_size.y, m_size.z);
    glTextureSubImage3D(glTextureId, 0, 0, 0, 0, m_size.x, m_size.y, m_size.z,
                        load[0].metaData.format, load[0].metaData.type, load[0].pixelData);
}

// ---- Cube ---------------------------------------------------------------------------------------

void OpenGLTensorBufferCubeBit::setSize(int width, int height, int count)
{
    m_size = width;
    if (m_size != height) {
        throw std::runtime_error("Cube texture must be square");
    }
    if (count != 6) {
        throw std::runtime_error("Cube texture must have 6 layers");
    }
}

void OpenGLTensorBufferCubeBit::loadDataToGPU(const GpuFormatInfo &storage,
                                              std::span<const LoadInfo> load)
{
    if (load.size() != 6) {
        throw std::runtime_error(
            "OpenGLTensorBufferCubeBit::loadDataToGPU: number of buffers must be 6");
    }
    glTextureStorage2D(glTextureId, 1, storage.internalFormat, m_size, m_size);
    for (int i = 1; i < 6; i++) {
        glTextureSubImage3D(glTextureId, 0, 0, 0, i, m_size, m_size, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

// ---- 1D layered ---------------------------------------------------------------------------------

void OpenGLTensorBuffer1DLayeredBit::setSize(int width, int height, int count)
{
    m_size = width;
    m_layerCount = count;
}

void OpenGLTensorBuffer1DLayeredBit::loadDataToGPU(const GpuFormatInfo &storage,
                                                   std::span<const LoadInfo> load)
{
    glTextureStorage2D(glTextureId, 1, storage.internalFormat, m_size, m_layerCount);
    for (int i = 0; i < m_layerCount; i++) {
        glTextureSubImage2D(glTextureId, 0, 0, i, m_size, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

// ---- 2D layered ---------------------------------------------------------------------------------

void OpenGLTensorBuffer2DLayeredBit::setSize(int width, int height, int count)
{
    m_size = {width, height};
    m_layerCount = count;
}

void OpenGLTensorBuffer2DLayeredBit::loadDataToGPU(const GpuFormatInfo &storage,
                                                   std::span<const LoadInfo> load)
{
    glTextureStorage3D(glTextureId, 1, storage.internalFormat, m_size.x, m_size.y, m_layerCount);
    for (int i = 0; i < m_layerCount; i++) {
        glTextureSubImage3D(glTextureId, 0, 0, 0, i, m_size.x, m_size.y, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

// ---- Cube layered -------------------------------------------------------------------------------

void OpenGLTensorBufferCubeLayeredBit::setSize(int width, int height, int count)
{
    m_size = width;
    if (m_size != height) {
        throw std::runtime_error("Cube texture must be square");
    }
    if (count % 6 != 0) {
        throw std::runtime_error("2D layered texture must have multiple of 6 images");
    }
    m_layerCount = count / 6;
}

void OpenGLTensorBufferCubeLayeredBit::loadDataToGPU(const GpuFormatInfo &storage,
                                                     std::span<const LoadInfo> load)
{
    glTextureStorage3D(glTextureId, 1, storage.internalFormat, m_size, m_size, m_layerCount * 6);
    for (int i = 0; i < m_layerCount * 6; i++) {
        glTextureSubImage3D(glTextureId, 0, 0, 0, i, m_size, m_size, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

} // namespace Vitrae