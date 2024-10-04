#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

#include "Graphics/GraphicsEngine/RHI/RHIStructs.h"

struct TrailVertex
{
	CU::Vector4f Position;
	CU::Vector4f Color;
	float Lifetime;
	float Width;
	CU::Vector4f ChannelMask;

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};