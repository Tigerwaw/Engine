#pragma once
#include <vector>
#include "GameEngine/Math/Vector.hpp"

struct NavNode
{
	CU::Vector3f position;
	std::vector<int> portals;
	bool isPassable = true;
};
