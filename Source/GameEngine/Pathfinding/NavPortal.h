#pragma once
#include "Math/Vector.hpp"
#include <array>

struct NavPortal
{
	std::array<int, 2> nodes;
	std::array<Math::Vector3f, 2> vertices;
	float cost = 0;

	const int GetOtherNode(const int aOwnNodeIndex) const
	{
		return (nodes[0] == aOwnNodeIndex) ? nodes[1] : nodes[0];
	}

	const bool IsSameEdge(const Math::Vector3f& aPointOne, const Math::Vector3f& aPointTwo) const
	{
		if (aPointOne == vertices[0] && aPointTwo == vertices[1]) return true;
		if (aPointOne == vertices[1] && aPointTwo == vertices[0]) return true;

		return false;
	}
};