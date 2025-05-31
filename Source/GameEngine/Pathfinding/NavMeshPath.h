#pragma once
#include <vector>
#include "Math/Vector.hpp"


class NavMeshPath
{
public:
	NavMeshPath() = default;
	NavMeshPath(std::vector<Math::Vector3f>&& aPath);

	const int GetSize() const;
	const bool Empty() const;

	Math::Vector3f operator[](int aPoint) const;
	const Math::Vector3f& operator[](int aPoint);
private:
	std::vector<Math::Vector3f> myPath;
};