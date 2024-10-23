#pragma once
#include <vector>
#include "GameEngine/Math/Vector.hpp"

struct NavNode
{
	friend class NavMesh;

	CU::Vector3f position;
	std::vector<int> edges;
	std::vector<int> edgeCosts;
	bool isPassable = true;
private:
	int currentDistance = INT_MAX;
	int predecessor = -1;
	bool hasBeenChecked = false;
};
