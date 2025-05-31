#pragma once
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "EngineDefines.h"



struct SpriteBuffer
{
	Math::Matrix4x4f Matrix;				// 64
	float CurrentFrame = 0;				// 4
	Math::Vector2f SpriteSheetDimensions;	// 8
	float Padding = 0;					// 4
};