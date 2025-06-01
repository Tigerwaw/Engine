#pragma once
#include "Math/Vector.hpp"
#include "Math/Curve.h"

struct ParticleEmitterSettings
{
	Math::Vector3f EmitterSize;
	float SpawnRate = 1.0f;
	Math::Curve<Math::Vector4f> Color;

	Math::Curve<Math::Vector2f> Size;
	Math::Vector3f Velocity;
	bool RandomVelocity = false;
	Math::Vector3f VelocityRangeMin;
	Math::Vector3f VelocityRangeMax;

	float Lifetime = 5.0f;
	float GravityScale = 1.0f;
	Math::Curve<float> Angle;
	Math::Vector4f ChannelMask = { 1.0f, 0, 0, 0 };
};