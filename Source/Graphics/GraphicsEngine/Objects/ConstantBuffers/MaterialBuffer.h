#pragma once
#include "GameEngine/Math/Vector.hpp"

namespace CU = CommonUtilities;

struct MaterialBuffer
{
	CU::Vector4<float> albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	float emissiveStrength = 0.0f;
	CU::Vector3f padding;
};