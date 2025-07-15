#pragma once
#include "Math/Vector.hpp"

struct MaterialBuffer
{
	Math::Vector4<float> albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	float emissiveStrength = 0.0f;
	Math::Vector3f padding;
};