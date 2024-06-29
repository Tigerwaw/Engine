#pragma once
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"
#include "GameEngine/EngineDefines.h"

namespace CU = CommonUtilities;

struct SpriteBuffer
{
	CU::Vector4f Position;	// 4
	CU::Vector2f Size;		// 2
	unsigned IsScreenSpace;	// 1
	float Padding;			// 1
};