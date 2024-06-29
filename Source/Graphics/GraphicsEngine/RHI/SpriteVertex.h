#pragma once
#include <vector>
#include "GameEngine\Math\Vector.hpp"

#include "RHIStructs.h"

namespace CU = CommonUtilities;

struct SpriteVertex
{
	CU::Vector4f Position = { 0, 0, 0, 1 };
	CU::Vector2f Size = { 0, 0 };

	SpriteVertex() = default;

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

