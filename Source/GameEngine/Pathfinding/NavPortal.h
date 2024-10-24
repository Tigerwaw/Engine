#pragma once
#include "GameEngine/Math/Vector.hpp"
#include <array>

struct NavPortal
{
	std::array<int, 2> nodes;
	std::array<CU::Vector3f, 2> vertices;
	float cost = 0;

	const int GetOtherNode(const int& aOwnNodeIndex) const
	{
		if (nodes[0] == aOwnNodeIndex)
		{
			return nodes[1];
		}
		else
		{
			return nodes[0];
		}
	}

	const bool IsSameEdge(const CU::Vector3f& aPointOne, const CU::Vector3f& aPointTwo) const
	{
		if (aPointOne == vertices[0] && aPointTwo == vertices[1]) return true;
		if (aPointOne == vertices[1] && aPointTwo == vertices[0]) return true;

		return false;
	}
};