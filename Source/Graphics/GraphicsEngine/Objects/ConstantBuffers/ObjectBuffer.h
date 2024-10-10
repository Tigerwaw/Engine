#pragma once
#include "GameEngine/Math/Matrix.hpp"
namespace CU = CommonUtilities;

struct ObjectBuffer
{
	CU::Matrix4x4f World;
	CU::Matrix4x4f WorldInvT;
	unsigned hasSkinning = 0;
	unsigned isInstanced = 0;
	CU::Vector2f padding;
	CU::Vector4f customData_1;
	CU::Vector4f customData_2;
};