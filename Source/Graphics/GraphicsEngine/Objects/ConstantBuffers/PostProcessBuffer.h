#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

struct PostProcessBuffer
{
	CU::Vector4f RandomKernel[64];
	unsigned KernelSize = 64;
	unsigned SSAOEnabled = 1;
	unsigned BloomFunction = 0;
	unsigned LuminanceFunction = 0;
	float SSAONoisePower = 0.25f;
	float SSAORadius = 0.1f;
	float SSAOBias = 0.025f;
	float BloomStrength = 1.0f;
};