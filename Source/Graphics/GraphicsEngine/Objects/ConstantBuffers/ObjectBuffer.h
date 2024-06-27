#pragma once
#include "GameEngine/Math/Matrix.hpp"

struct ObjectBuffer
{
	CommonUtilities::Matrix4x4f World;
	CommonUtilities::Matrix4x4f WorldInvT;
	unsigned hasSkinning;
	CommonUtilities::Vector3f Padding;
};