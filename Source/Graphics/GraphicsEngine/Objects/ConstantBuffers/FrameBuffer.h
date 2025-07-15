#pragma once
#include "Math/Matrix.hpp"

struct FrameBuffer
{
	Math::Matrix4x4f InvView;
	Math::Matrix4x4f Projection;
	Math::Vector4f ViewPosition;
	Math::Vector4f ViewDirection;
	float NearPlane = 0;
	float FarPlane = 0;
	int DebugRenderMode = 0;
	float Padding;
	Math::Vector2f Time; // x = TotalTime, y = Delta Time
	Math::Vector2f Resolution;
};