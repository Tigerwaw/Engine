#pragma once
#include "Math/Vector.hpp"
#include "RHI/RHIStructs.h"

struct ParticleVertex
{
	Math::Vector4f Position = { 0, 0, 0, 1 };
	Math::Vector4f Color = { 1, 1, 1, 1 };
	Math::Vector3f Velocity;
	float GravityScale;
	float Lifetime;
	float Angle;
	Math::Vector2f Size;
	Math::Vector4f ChannelMask;

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

