#pragma once

#include "Vitrae/Assets/Shapes/Shape.hpp"
#include "Vitrae/Setup/Rasterizing.hpp"

namespace Vitrae
{
void stateSetupRasterizing(const RasterizingSetupParams &params);

void rasterizeShape(const Shape &shape, const RasterizingSetupParams &params);
}