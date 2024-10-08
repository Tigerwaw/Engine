#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

#include "Graphics/GraphicsEngine/RHI/RHIStructs.h"

struct ParticleVertex
{
	CU::Vector4f Position = { 0, 0, 0, 1 };
	CU::Vector4f Color = { 1, 1, 1, 1 };
	CU::Vector3f Velocity;
	float GravityScale;
	float Lifetime;
	float Angle;
	CU::Vector2f Size;
	CU::Vector4f ChannelMask;

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

