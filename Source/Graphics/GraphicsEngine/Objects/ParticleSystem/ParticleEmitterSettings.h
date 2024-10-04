#pragma once
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Curve.h"
namespace CU = CommonUtilities;

struct ParticleEmitterSettings
{
	CU::Vector3f EmitterSize;
	float SpawnRate = 1.0f;
	CU::Curve<CU::Vector4f> Color;

	CU::Curve<CU::Vector2f> Size;
	CU::Vector3f Velocity;
	bool RandomVelocity = false;
	CU::Vector3f VelocityRangeMin;
	CU::Vector3f VelocityRangeMax;

	float Lifetime = 5.0f;
	float GravityScale = 1.0f;
	CU::Curve<float> Angle;
	CU::Vector4f ChannelMask = { 1.0f, 0, 0, 0 };
};