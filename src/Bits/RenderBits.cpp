#include "VitraePluginOpenGL/Bits/RenderBits.hpp"

#include "glad/glad.h"
// must be after glad.h
#include "GLFW/glfw3.h"

#include "MMeter.h"

namespace Vitrae
{
void stateSetupRasterizing(const RasterizingSetupParams &params)
{
    MMETER_FUNC_PROFILER;

    {
        MMETER_SCOPE_PROFILER("General setup");

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

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
            glLineWidth(1.5);
        } else {
            glDisable(GL_LINE_SMOOTH);
            glLineWidth(1.0);
        }

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
        glBlendFunc(convertBlending(params.sourceBlending),
                    convertBlending(params.destinationBlending));
        if (params.sourceBlending == BlendingFunction::One &&
            params.destinationBlending == BlendingFunction::Zero) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
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
