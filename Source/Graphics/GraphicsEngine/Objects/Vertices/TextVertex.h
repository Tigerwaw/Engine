#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

#include "Graphics/GraphicsEngine/RHI/RHIStructs.h"

struct TextVertex
{
	CU::Vector4f Position;
	CU::Vector2f TexCoords;

	TextVertex(float positionX, float positionY, float uvX, float uvY)
	{
		Position = { positionX, positionY, 0, 1 };
		TexCoords = { uvX, uvY };
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

