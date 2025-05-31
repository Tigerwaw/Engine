#pragma once
#include <vector>
#include "Math/Vector.hpp"

struct NavNode
{
	Math::Vector3f position;
	std::vector<int> portals;
	bool isPassable = true;
};
