#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

#include "Graphics/GraphicsEngine/RHI/RHIStructs.h"

struct TextVertex
{
	CU::Vector4<float> Position = { 0, 0, 0, 1 };
	CU::Vector2<float> TexCoords;

	TextVertex(float position[3], float UVs[2])
	{
		Position = { position[0], position[1], position[2], 1 };
		TexCoords = { UVs[0], UVs[1] };
	}

	TextVertex(float positionX, float positionY, float uvX, float uvY)
	{
		Position = { positionX, positionY, 0, 1 };
		TexCoords = { uvX, uvY };
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

