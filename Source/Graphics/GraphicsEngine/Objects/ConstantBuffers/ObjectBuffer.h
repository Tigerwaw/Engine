#pragma once
#include "Math/Matrix.hpp"


struct ObjectBuffer
{
	Math::Matrix4x4f World;
	Math::Matrix4x4f WorldInvT;
	unsigned hasSkinning = 0;
	unsigned isInstanced = 0;
	Math::Vector2f padding;
	Math::Vector4f customData_1;
	Math::Vector4f customData_2;
};