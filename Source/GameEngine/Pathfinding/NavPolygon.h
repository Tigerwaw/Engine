#pragma once
#include <vector>
#include "GameEngine/Math/Vector.hpp"

struct NavPolygon
{
	std::array<CU::Vector3f, 3> vertexPositions;
};