#pragma once
#include "Math/Matrix.hpp"


struct Animation
{
	struct Frame
	{
		std::unordered_map<std::string, Math::Matrix4x4f> BoneTransforms;
	};

	std::vector<Frame> Frames;
	float Duration = 0;
	float FramesPerSecond = 0;
};