#pragma once
#include <vector>
#include "GameEngine\Math\Vector.hpp"

#include "RHIStructs.h"

namespace CU = CommonUtilities;

struct SpriteVertex
{
	CU::Vector4<float> Position = { 0, 0, 0, 1 };
	CU::Vector2<float> Size = { 1, 1 };

	SpriteVertex(float position[4], float size[2])
	{
		Position = { position[0], position[1], position[2], 1 };
		Size = { size[0], size[1] };
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

