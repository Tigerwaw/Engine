#pragma once
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"
#include "GameEngine/EngineDefines.h"

namespace CU = CommonUtilities;

struct SpriteBuffer
{
	CU::Matrix4x4f Matrix;				// 64
	float CurrentFrame = 0;				// 4
	CU::Vector2f SpriteSheetDimensions;	// 8
	float Padding = 0;					// 4
};