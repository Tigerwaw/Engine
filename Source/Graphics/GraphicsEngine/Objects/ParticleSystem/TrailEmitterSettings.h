#pragma once
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Curve.h"
namespace CU = CommonUtilities;

struct TrailEmitterSettings
{
	unsigned Vertices = 2;
	CU::Curve<CU::Vector4f> Color;
	CU::Curve<float> Width;
	unsigned Length = 10;
	CU::Vector4f ChannelMask = { 1.0f, 0, 0, 0 };
};
