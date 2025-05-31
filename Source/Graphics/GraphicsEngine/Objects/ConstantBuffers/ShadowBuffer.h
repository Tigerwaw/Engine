#pragma once
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "EngineDefines.h"



struct ShadowBuffer
{
	Math::Matrix4x4f CameraTransforms[6]; // 64 * 6 = 384 bytes
};