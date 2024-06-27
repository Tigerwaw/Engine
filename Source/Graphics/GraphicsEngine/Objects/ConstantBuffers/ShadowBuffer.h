#pragma once
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"
#include "GameEngine/EngineDefines.h"

namespace CU = CommonUtilities;

struct ShadowBuffer
{
	CU::Matrix4x4f CameraTransforms[6]; // 64 * 6 = 384 bytes
};