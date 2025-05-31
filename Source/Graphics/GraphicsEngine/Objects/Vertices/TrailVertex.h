#pragma once
#include "Math/Vector.hpp"


#include "RHI/RHIStructs.h"

struct TrailVertex
{
	Math::Vector4f Position = { 0, 0, 0, 1 };
	Math::Vector4f Color = { 1, 1, 1, 1 };
	float Lifetime;
	float Width;
	Math::Vector4f ChannelMask;

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};