#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "GameEngine/Math/Matrix.hpp"

struct Animation
{
	struct Frame
	{
		std::unordered_map<std::string, CommonUtilities::Matrix4x4<float>> BoneTransforms;
	};

	std::vector<Frame> Frames;
	float Duration;
	float FramesPerSecond;
};