#pragma once
#include "GameEngine/Math/Matrix.hpp"
namespace CU = CommonUtilities;

struct ObjectBuffer
{
	CU::Matrix4x4f World;
	CU::Matrix4x4f WorldInvT;
	unsigned hasSkinning;
	CU::Vector3f customData_1;
	CU::Vector4f customData_2;
};