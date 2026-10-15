#include "VitraePluginOpenGL/Specializations/PixelImage.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Data/PixelTyping.hpp"
#include "Vitrae/Setup/TextureFiltering.hpp"

#include "stb/stb_image.h"

namespace Vitrae
{
// ==== Helpers ====================================================================================

namespace
{

GLint PIXEL_TYPE_2_GL_CPU_FORMAT[] = {
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

GLint PIXEL_TYPE_2_GL_CPU_TYPE[] = {
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

GLint *PIXEL_FORMAT_2_GL_INTERNAL_FORMAT[] = {
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

GLint getInternalFormat(AnyPixelFormat pixelFormat)
{
    // PixelType corresponds to the active index of the variant
    std::size_t pixelTypeIndex = pixelFormat.index();
    // PixelFormat retrieval *should* get optimized into just getting an integer since the same
    // code runs for each option
    std::size_t pixelFormatIndex = std::visit(
        [&](auto &&pixelType) { return static_cast<std::size_t>(pixelType); }, pixelFormat);

    return PIXEL_FORMAT_2_GL_INTERNAL_FORMAT[pixelTypeIndex][pixelFormatIndex];
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

GpuFormatInfo getGpuFormatInfo(PixelType internalType, int stbChannelFormat, GLenum dataType)
{
    GpuFormatInfo ret;

    switch (internalType) {
    case PixelType::REAL_SCALAR:
        ret.useSwizzle = false;

        switch (dataType) {
        case GL_FLOAT:
            ret.internalFormat = GL_R32F;
            break;
        case GL_UNSIGNED_SHORT:
            ret.internalFormat = GL_R16;
            break;
        case GL_UNSIGNED_BYTE:
            ret.internalFormat = GL_R8;
            break;
        default:
            ret.internalFormat = GL_INVALID_ENUM;
            break;
        }
        break;
    case PixelType::REAL_VEC2:
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
                ret.internalFormat = GL_R8;
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
                ret.internalFormat = GL_RG8;
                break;
            default:
                ret.internalFormat = GL_INVALID_ENUM;
                break;
            }
        }
        break;
    case PixelType::REAL_VEC3:
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
                ret.internalFormat = GL_R8;
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
                ret.internalFormat = GL_RGB8;
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
    case PixelType::REAL_VEC4:
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
                ret.internalFormat = GL_R8;
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
                ret.internalFormat = GL_RG8;
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
                ret.internalFormat = GL_RGB8;
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
                ret.internalFormat = GL_RGBA8;
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
        // PixelType corresponds to the active index of the variant
        std::size_t pixelTypeIndex = (std::size_t)internalType;

        // Use the default/first format per buffer type, should be good enough
        std::size_t pixelFormatIndex = 0;

        ret.internalFormat = PIXEL_FORMAT_2_GL_INTERNAL_FORMAT[pixelTypeIndex][pixelFormatIndex];
        break;
    }
    }

    return ret;
}

GpuFormatInfo getGpuFormatInfo(AnyPixelFormat pixelFormat, glm::vec<4, ChannelSource> swizzleSpec)
{
    GpuFormatInfo ret;

    ret.internalFormat = getInternalFormat(pixelFormat);

    PixelType pixelType = static_cast<PixelType>(pixelFormat.index());

    ret.swizzleVal = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA};

    for (int i = 0; i < getChannelCount(pixelType); ++i) {
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

CpuDataInfo getNativeCpuDataInfo(PixelType pixelType)
{
    CpuDataInfo ret;
    std::size_t componentBits, componentCount;

    switch (pixelType) {
    case PixelType::REAL_SCALAR:
    case PixelType::REAL_VEC2:
    case PixelType::REAL_VEC3:
    case PixelType::REAL_VEC4:
        ret.type = GL_FLOAT;
        componentBits = sizeof(GLfloat) * 8;
        break;
    case PixelType::WHOLE_SCALAR:
    case PixelType::WHOLE_VEC2:
    case PixelType::WHOLE_VEC3:
    case PixelType::WHOLE_VEC4:
        ret.type = GL_INT;
        componentBits = sizeof(GLint) * 8;
        break;
    case PixelType::COUNT_SCALAR:
    case PixelType::COUNT_VEC2:
    case PixelType::COUNT_VEC3:
    case PixelType::COUNT_VEC4:
        ret.type = GL_UNSIGNED_INT;
        componentBits = sizeof(GLuint) * 8;
        break;
    case PixelType::DEPTH:
        ret.type = GL_FLOAT;
        componentBits = sizeof(GLfloat) * 8;
        break;
    case PixelType::STENCIL:
        ret.type = GL_UNSIGNED_INT;
        componentBits = sizeof(GLuint) * 8;
        break;
    case PixelType::DEPTH_AND_STENCIL:
        ret.type = GL_INVALID_ENUM;
        componentBits = 0;
        break;
    }

    switch (pixelType) {
    case PixelType::REAL_SCALAR:
    case PixelType::WHOLE_SCALAR:
    case PixelType::COUNT_SCALAR:
    case PixelType::DEPTH:
    case PixelType::STENCIL:
        ret.format = GL_RED;
        componentCount = 1;
        break;
    case PixelType::REAL_VEC2:
    case PixelType::WHOLE_VEC2:
    case PixelType::COUNT_VEC2:
        ret.format = GL_RG;
        componentCount = 2;
        break;
    case PixelType::REAL_VEC3:
    case PixelType::WHOLE_VEC3:
    case PixelType::COUNT_VEC3:
        ret.format = GL_RGB;
        componentCount = 3;
        break;
    case PixelType::REAL_VEC4:
    case PixelType::WHOLE_VEC4:
    case PixelType::COUNT_VEC4:
        ret.format = GL_RGBA;
        componentCount = 4;
        break;
    case PixelType::DEPTH_AND_STENCIL:
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
    bool useMipMaps;

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

    if (std::holds_alternative<bool>(filtering.useMipMaps) &&
        std::holds_alternative<FilterType>(filtering.mipmapFilter)) {
        useMipMaps = std::get<bool>(filtering.useMipMaps);

        if (std::holds_alternative<FilterType>(filtering.minFilter)) {
            switch (std::get<FilterType>(filtering.minFilter)) {
            case FilterType::LINEAR:
                switch (std::get<FilterType>(filtering.mipmapFilter)) {
                case FilterType::LINEAR:
                    glMinFilter = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                case FilterType::NEAREST:
                    glMinFilter = GL_LINEAR_MIPMAP_NEAREST;
                    break;
                }
                break;
            case FilterType::NEAREST:
                switch (std::get<FilterType>(filtering.minFilter)) {
                case FilterType::LINEAR:
                    glMinFilter = GL_NEAREST_MIPMAP_LINEAR;
                    break;
                case FilterType::NEAREST:
                    glMinFilter = GL_NEAREST_MIPMAP_NEAREST;
                    break;
                }
                break;
            }

        } else {
            switch (std::get<FilterType>(filtering.mipmapFilter)) {
            case FilterType::LINEAR:
                glMinFilter = GL_NEAREST_MIPMAP_LINEAR;
                break;
            case FilterType::NEAREST:
                glMinFilter = GL_NEAREST_MIPMAP_NEAREST;
                break;
            }
        }
    } else {
        useMipMaps = false;

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
    }

    if (std::holds_alternative<glm::vec4>(filtering.borderColor)) {
        glTextureParameterfv(glTextureId, GL_TEXTURE_BORDER_COLOR,
                             &std::get<glm::vec4>(filtering.borderColor)[0]);
    }

    if (formatInfo.useSwizzle) {
        glTextureParameteriv(glTextureId, GL_TEXTURE_SWIZZLE_RGBA, formatInfo.swizzleArr);
    }

    glTextureParameteri(glTextureId, GL_TEXTURE_MAG_FILTER, glMagFilter);
    glTextureParameteri(glTextureId, GL_TEXTURE_MIN_FILTER, glMinFilter);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_S, glWrapS);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_T, glWrapT);
}

} // namespace

OpenGLPixelImageBit::OpenGLPixelImageBit() {}

OpenGLPixelImageBit::~OpenGLPixelImageBit()
{
    glDeleteTextures(1, &glTextureId);
}

std::size_t OpenGLPixelImageBit::gpuMemoryCostBit() const
{
    return 0;
}

void OpenGLPixelImageBit::setupFromFile(PixelType pixelType,
                                          const ImageCommon::FileLoadParams &params)
{
    m_sentToGPU = true;
    glCreateTextures(GL_TEXTURE_2D, 1, &glTextureId);

    int stbChannelFormat, width, height;
    LoadInfo load[] = {
        readImageFile(params.root, params.filepath, width, height, stbChannelFormat),
    };

    GpuFormatInfo storageInfo =
        getGpuFormatInfo(pixelType, stbChannelFormat, load[0].metaData.type);

    bool useMipmaps = std::holds_alternative<bool>(params.filtering.useMipMaps) &&
                      std::get<bool>(params.filtering.useMipMaps);

    setSize(width, height, 1);
    setupStorageOnGPU(storageInfo, useMipmaps);
    loadDataToGPU(load);

    if (useMipmaps) {
        glGenerateTextureMipmap(glTextureId);
    }

    loadFiltering(glTextureId, storageInfo, params.filtering);

    stbi_image_free(load[0].pixelData);

    String glLabel = String("texture ") + params.filepath.c_str();
    glBindTexture(GL_TEXTURE_2D, glTextureId);
    glObjectLabel(GL_TEXTURE, glTextureId, glLabel.size(), glLabel.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLPixelImageBit::setupFromColor(AnyPixelFormat pixelFormat, const void *pixelData)
{
    m_sentToGPU = true;
    glCreateTextures(GL_TEXTURE_2D, 1, &glTextureId);

    PixelType pixelType = static_cast<PixelType>(pixelFormat.index());

    GpuFormatInfo storageInfo = {
        .useSwizzle = false,

        // Use default (first) format for this pixelType
        .internalFormat = PIXEL_FORMAT_2_GL_INTERNAL_FORMAT[(int)pixelType][0],
    };
    LoadInfo load[] = {
        {
            .metaData = getNativeCpuDataInfo(pixelType),
            .constPixelData = pixelData,
        },
    };

    glTextureParameteri(glTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(glTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(glTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    setSize(1, 1, 1);
    setupStorageOnGPU(storageInfo, false);
    loadDataToGPU(load);

    /// TODO: color name
    String glLabel = String("color ") + std::to_string((std::size_t)this);
    glBindTexture(GL_TEXTURE_2D, glTextureId);
    glObjectLabel(GL_TEXTURE, glTextureId, glLabel.size(), glLabel.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLPixelImageBit::setupEmpty(const glm::uvec4 &ndSize, AnyPixelFormat pixelFormat,
                                       const glm::vec<4, ChannelSource> &swizzleSpec,
                                       const TextureFilteringParams &filtering, String friendlyName)
{
    m_sentToGPU = false;
    glCreateTextures(GL_TEXTURE_2D, 1, &glTextureId);

    GpuFormatInfo storageInfo = getGpuFormatInfo(pixelFormat, swizzleSpec);

    loadFiltering(glTextureId, storageInfo, filtering);
    setSize(ndSize);
    setupStorageOnGPU(storageInfo, false);

    String glLabel = String("empty ") + friendlyName;
    glBindTexture(GL_TEXTURE_2D, glTextureId);
    glObjectLabel(GL_TEXTURE, glTextureId, glLabel.size(), glLabel.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ==== Texture dimension bits =====================================================================

// ---- 1D -----------------------------------------------------------------------------------------

void OpenGLPixelImage1DBit::setSize(int width, int height, int count)
{
    m_size = width;
    if (height != 1) {
        throw std::runtime_error("1D texture can have only height == 1");
    }
    if (count != 1) {
        throw std::runtime_error("1D texture can have only 1 layer");
    }
}

void OpenGLPixelImage1DBit::setSize(glm::uvec4 ndSize)
{
    m_size = ndSize.x;
}

void OpenGLPixelImage1DBit::setupStorageOnGPU(const GpuFormatInfo &storage, bool useMipmaps)
{
    glTextureStorage1D(glTextureId, useMipmaps ? (1 + floor(log2(m_size))) : 1,
                       storage.internalFormat, m_size);
}

void OpenGLPixelImage1DBit::loadDataToGPU(std::span<const LoadInfo> load)
{
    if (load.size() != 1) {
        throw std::runtime_error(
            "OpenGLPixelImage1DBit::loadDataToGPU: number of buffers must be 1");
    }
    glTextureSubImage1D(glTextureId, 0, 0, m_size, load[0].metaData.format, load[0].metaData.type,
                        load[0].pixelData);
}

// ---- 2D -----------------------------------------------------------------------------------------

void OpenGLPixelImage2DBit::setSize(int width, int height, int count)
{
    m_size = glm::uvec2(width, height);
    if (count != 1) {
        throw std::runtime_error("2D texture can have only 1 layer");
    }
}

void OpenGLPixelImage2DBit::setSize(glm::uvec4 ndSize)
{
    m_size = {ndSize.x, ndSize.y};
}

void OpenGLPixelImage2DBit::setupStorageOnGPU(const GpuFormatInfo &storage, bool useMipmaps)
{
    glTextureStorage2D(glTextureId,
                       useMipmaps ? (1 + floor(log2(std::max(m_size.x, m_size.y)))) : 1,
                       storage.internalFormat, m_size.x, m_size.y);
}

void OpenGLPixelImage2DBit::loadDataToGPU(std::span<const LoadInfo> load)
{
    if (load.size() != 1) {
        throw std::runtime_error(
            "OpenGLPixelImage2DBit::loadDataToGPU: number of buffers must be 1");
    }
    glTextureSubImage2D(glTextureId, 0, 0, 0, m_size.x, m_size.y, load[0].metaData.format,
                        load[0].metaData.type, load[0].pixelData);
}

// ---- 3D -----------------------------------------------------------------------------------------

void OpenGLPixelImage3DBit::setSize(int width, int height, int count)
{
    m_size = glm::uvec3(width, height, 1);
}

void OpenGLPixelImage3DBit::setSize(glm::uvec4 ndSize)
{
    m_size = {ndSize.x, ndSize.y, ndSize.z};
}

void OpenGLPixelImage3DBit::setupStorageOnGPU(const GpuFormatInfo &storage, bool useMipmaps)
{
    glTextureStorage3D(
        glTextureId,
        useMipmaps ? (1 + floor(log2(std::max(m_size.x, std::max(m_size.y, m_size.z))))) : 1,
        storage.internalFormat, m_size.x, m_size.y, m_size.z);
}

void OpenGLPixelImage3DBit::loadDataToGPU(std::span<const LoadInfo> load)
{
    if (load.size() != 1) {
        throw std::runtime_error(
            "OpenGLPixelImage3DBit::loadDataToGPU: number of buffers must be 1");
    }
    glTextureSubImage3D(glTextureId, 0, 0, 0, 0, m_size.x, m_size.y, m_size.z,
                        load[0].metaData.format, load[0].metaData.type, load[0].pixelData);
}

// ---- Cube ---------------------------------------------------------------------------------------

void OpenGLPixelImageCubeBit::setSize(int width, int height, int count)
{
    m_size = width;
    if (m_size != height) {
        throw std::runtime_error("Cube texture must be square");
    }
    if (count != 6) {
        throw std::runtime_error("Cube texture must have 6 layers");
    }
}

void OpenGLPixelImageCubeBit::setSize(glm::uvec4 ndSize)
{
    m_size = ndSize.x;
}

void OpenGLPixelImageCubeBit::setupStorageOnGPU(const GpuFormatInfo &storage, bool useMipmaps)
{
    glTextureStorage2D(glTextureId, useMipmaps ? (1 + floor(log2(m_size))) : 1,
                       storage.internalFormat, m_size, m_size);
}

void OpenGLPixelImageCubeBit::loadDataToGPU(std::span<const LoadInfo> load)
{
    if (load.size() != 6) {
        throw std::runtime_error(
            "OpenGLPixelImageCubeBit::loadDataToGPU: number of buffers must be 6");
    }
    for (int i = 1; i < 6; i++) {
        glTextureSubImage3D(glTextureId, 0, 0, 0, i, m_size, m_size, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

// ---- 1D layered ---------------------------------------------------------------------------------

void OpenGLPixelImage1DLayeredBit::setSize(int width, int height, int count)
{
    m_size = width;
    m_layerCount = count;
}

void OpenGLPixelImage1DLayeredBit::setSize(glm::uvec4 ndSize)
{
    m_size = ndSize.x;
    m_layerCount = ndSize.y;
}

void OpenGLPixelImage1DLayeredBit::setupStorageOnGPU(const GpuFormatInfo &storage, bool useMipmaps)
{
    glTextureStorage2D(glTextureId, useMipmaps ? (1 + floor(log2(m_size))) : 1,
                       storage.internalFormat, m_size, m_layerCount);
}

void OpenGLPixelImage1DLayeredBit::loadDataToGPU(std::span<const LoadInfo> load)
{
    for (int i = 0; i < m_layerCount; i++) {
        glTextureSubImage2D(glTextureId, 0, 0, i, m_size, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

// ---- 2D layered ---------------------------------------------------------------------------------

void OpenGLPixelImage2DLayeredBit::setSize(int width, int height, int count)
{
    m_size = {width, height};
    m_layerCount = count;
}

void OpenGLPixelImage2DLayeredBit::setSize(glm::uvec4 ndSize)
{
    m_size = {ndSize.x, ndSize.y};
    m_layerCount = ndSize.z;
}

void OpenGLPixelImage2DLayeredBit::setupStorageOnGPU(const GpuFormatInfo &storage, bool useMipmaps)
{
    glTextureStorage3D(glTextureId,
                       useMipmaps ? (1 + floor(log2(std::max(m_size.x, m_size.y)))) : 1,
                       storage.internalFormat, m_size.x, m_size.y, m_layerCount);
}

void OpenGLPixelImage2DLayeredBit::loadDataToGPU(std::span<const LoadInfo> load)
{
    for (int i = 0; i < m_layerCount; i++) {
        glTextureSubImage3D(glTextureId, 0, 0, 0, i, m_size.x, m_size.y, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

// ---- Cube layered -------------------------------------------------------------------------------

void OpenGLPixelImageCubeLayeredBit::setSize(int width, int height, int count)
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

void OpenGLPixelImageCubeLayeredBit::setSize(glm::uvec4 ndSize)
{
    m_size = ndSize.x;
    m_layerCount = ndSize.y;
}

void OpenGLPixelImageCubeLayeredBit::setupStorageOnGPU(const GpuFormatInfo &storage,
                                                       bool useMipmaps)
{
    glTextureStorage3D(glTextureId, useMipmaps ? (1 + floor(log2(m_size))) : 1,
                       storage.internalFormat, m_size, m_size, m_layerCount * 6);
}

void OpenGLPixelImageCubeLayeredBit::loadDataToGPU(std::span<const LoadInfo> load)
{
    for (int i = 0; i < m_layerCount * 6; i++) {
        glTextureSubImage3D(glTextureId, 0, 0, 0, i, m_size, m_size, 1, load[i].metaData.format,
                            load[i].metaData.type, load[i].pixelData);
    }
}

} // namespace Vitrae