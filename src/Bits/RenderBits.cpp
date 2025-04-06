#include "VitraePluginOpenGL/Bits/RenderBits.hpp"

#include "glad/glad.h"
// must be after glad.h
#include "GLFW/glfw3.h"

namespace Vitrae
{

void stateSetBlending(const RasterizingSetupParams &params)
{
    auto convertBlending = [](BlendingFunction blending) {
        switch (blending) {
        case BlendingFunction::Zero:
            return GL_ZERO;
        case BlendingFunction::One:
            return GL_ONE;
        case BlendingFunction::SourceColor:
            return GL_SRC_COLOR;
        case BlendingFunction::OneMinusSourceColor:
            return GL_ONE_MINUS_SRC_COLOR;
        case BlendingFunction::DestinationColor:
            return GL_DST_COLOR;
        case BlendingFunction::OneMinusDestinationColor:
            return GL_ONE_MINUS_DST_COLOR;
        case BlendingFunction::SourceAlpha:
            return GL_SRC_ALPHA;
        case BlendingFunction::OneMinusSourceAlpha:
            return GL_ONE_MINUS_SRC_ALPHA;
        case BlendingFunction::DestinationAlpha:
            return GL_DST_ALPHA;
        case BlendingFunction::OneMinusDestinationAlpha:
            return GL_ONE_MINUS_DST_ALPHA;
        case BlendingFunction::ConstantColor:
            return GL_CONSTANT_COLOR;
        case BlendingFunction::OneMinusConstantColor:
            return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendingFunction::ConstantAlpha:
            return GL_CONSTANT_ALPHA;
        case BlendingFunction::OneMinusConstantAlpha:
            return GL_ONE_MINUS_CONSTANT_ALPHA;
        case BlendingFunction::SourceAlphaSaturated:
            return GL_SRC_ALPHA_SATURATE;
        }
        return GL_ZERO;
    };

    glDepthMask(params.writeDepth);
    glBlendFunc(convertBlending(params.sourceBlending), convertBlending(params.destinationBlending));
    if (params.sourceBlending == BlendingFunction::One &&
        params.destinationBlending == BlendingFunction::Zero) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
    }
}

} // namespace Vitrae
