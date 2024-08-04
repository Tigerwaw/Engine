#pragma once
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"
#include "GameEngine/EngineDefines.h"

namespace CU = CommonUtilities;

struct SpriteBuffer
{
	CU::Matrix4x4f Matrix;				// 64
	unsigned IsScreenSpace;				// 4
	float CurrentFrame;					// 4
	CU::Vector2f SpriteSheetDimensions;	// 8
};