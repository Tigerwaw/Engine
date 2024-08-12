#pragma once
#include "GameEngine/Math/Matrix.hpp"
namespace CU = CommonUtilities;

struct Animation
{
	struct Frame
	{
		std::unordered_map<std::string, CU::Matrix4x4f> BoneTransforms;
	};

	std::vector<Frame> Frames;
	float Duration;
	float FramesPerSecond;
};