#pragma once
#include <unordered_map>

struct Vertex
{
	int currentDistance = INT_MAX;
	int predecessor = -1;
	std::unordered_map<int, int> edges;
	bool hasBeenChecked = false;
};
