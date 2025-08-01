#include "VitraePluginOpenGL/Bits/RenderBits.hpp"

#include "Vitrae/Data/Blending.hpp"
#include "Vitrae/Data/FragmentTest.hpp"
#include "glad/glad.h"
// must be after glad.h
#include "GLFW/glfw3.h"

#include "MMeter.h"

namespace Vitrae
{

namespace
{

auto convertTestFunction(FragmentTestFunction test)
{
    switch (test) {
    case FragmentTestFunction::Never:
        return GL_NEVER;
    case FragmentTestFunction::Less:
        return GL_LESS;
    case FragmentTestFunction::LessOrEqual:
        return GL_LEQUAL;
    case FragmentTestFunction::Equal:
        return GL_EQUAL;
    case FragmentTestFunction::NotEqual:
        return GL_NOTEQUAL;
    case FragmentTestFunction::GreaterOrEqual:
        return GL_GEQUAL;
    case FragmentTestFunction::Greater:
        return GL_GREATER;
    case FragmentTestFunction::Always:
        return GL_ALWAYS;
    }
}

auto convertBlendingOperation(BlendingOperation blending)
{
    switch (blending) {
    case BlendingOperation::Add:
        return GL_FUNC_ADD;
    case BlendingOperation::Subtract:
        return GL_FUNC_SUBTRACT;
    case BlendingOperation::ReverseSubtract:
        return GL_FUNC_REVERSE_SUBTRACT;
    case BlendingOperation::Min:
        return GL_MIN;
    case BlendingOperation::Max:
        return GL_MAX;
    }
};

auto convertBlendingFactor(BlendingFactor blending)
{
    switch (blending) {
    case BlendingFactor::Zero:
        return GL_ZERO;
    case BlendingFactor::One:
        return GL_ONE;
    case BlendingFactor::SourceColor:
        return GL_SRC_COLOR;
    case BlendingFactor::OneMinusSourceColor:
        return GL_ONE_MINUS_SRC_COLOR;
    case BlendingFactor::DestinationColor:
        return GL_DST_COLOR;
    case BlendingFactor::OneMinusDestinationColor:
        return GL_ONE_MINUS_DST_COLOR;
    case BlendingFactor::SourceAlpha:
        return GL_SRC_ALPHA;
    case BlendingFactor::OneMinusSourceAlpha:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BlendingFactor::DestinationAlpha:
        return GL_DST_ALPHA;
    case BlendingFactor::OneMinusDestinationAlpha:
        return GL_ONE_MINUS_DST_ALPHA;
    case BlendingFactor::ConstantColor:
        return GL_CONSTANT_COLOR;
    case BlendingFactor::OneMinusConstantColor:
        return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendingFactor::ConstantAlpha:
        return GL_CONSTANT_ALPHA;
    case BlendingFactor::OneMinusConstantAlpha:
        return GL_ONE_MINUS_CONSTANT_ALPHA;
    case BlendingFactor::SourceAlphaSaturated:
        return GL_SRC_ALPHA_SATURATE;
    }
};

} // namespace
void stateSetupRasterizing(const RasterizingSetupParams &params)
{
    MMETER_FUNC_PROFILER;

    {
        MMETER_SCOPE_PROFILER("General setup");

        if (params.depthTest == FragmentTestFunction::Always) {
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(convertTestFunction(params.depthTest));
        }

        switch (params.cullingMode) {
        case CullingMode::None:
            glDisable(GL_CULL_FACE);
            break;
        case CullingMode::Backface:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;
        case CullingMode::Frontface:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;
        }

        // smoothing
        if (params.smoothFilling) {
            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        } else {
            glDisable(GL_POLYGON_SMOOTH);
        }
        if (params.smoothTracing) {
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glLineWidth(params.lineWidth + 0.5);
        } else {
            glDisable(GL_LINE_SMOOTH);
            glLineWidth(params.lineWidth);
        }

        glDepthMask(params.writeDepth);
        if (params.blending == BlendingCommon::None) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendEquationSeparate(convertBlendingOperation(params.blending.operationRGB),
                                    convertBlendingOperation(params.blending.operationAlpha));
            glBlendFuncSeparate(convertBlendingFactor(params.blending.sourceRGB),
                                convertBlendingFactor(params.blending.destinationRGB),
                                convertBlendingFactor(params.blending.sourceAlpha),
                                convertBlendingFactor(params.blending.destinationAlpha));
        }
    }

    // Setup now for single-pass modes
    {
        MMETER_SCOPE_PROFILER("Mode setup");

        switch (params.rasterizingMode) {
        case RasterizingMode::DerivationalFillCenters:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case RasterizingMode::DerivationalTraceEdges:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case RasterizingMode::DerivationalDotVertices:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        default:
            break;
        }
    }
}

void rasterizeShape(const Shape &shape, const RasterizingSetupParams &params)
{
    switch (params.rasterizingMode) {
    case RasterizingMode::DerivationalFillCenters:
    case RasterizingMode::DerivationalTraceEdges:
    case RasterizingMode::DerivationalDotVertices:
        // Everything is already setup
        shape.rasterize();
        break;
    case RasterizingMode::DerivationalFillEdges:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        shape.rasterize();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shape.rasterize();
        break;
    case RasterizingMode::DerivationalFillVertices:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        shape.rasterize();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shape.rasterize();
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        shape.rasterize();
        break;
    case RasterizingMode::DerivationalTraceVertices:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        shape.rasterize();
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        shape.rasterize();
        break;
    }
}

} // namespace Vitrae
