#pragma once
#include "GameEngine/Math/Matrix.hpp"

struct FrameBuffer
{
	CommonUtilities::Matrix4x4f InvView;
	CommonUtilities::Matrix4x4f Projection;
	CommonUtilities::Vector4f ViewPosition;
	CommonUtilities::Vector4f ViewDirection;
	float NearPlane = 0;
	float FarPlane = 0;
	CommonUtilities::Vector2f Padding;
	CommonUtilities::Vector2f Time;
	CommonUtilities::Vector2f Resolution;
};