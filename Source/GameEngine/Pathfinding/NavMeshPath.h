#pragma once
#include <vector>
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class NavMeshPath
{
public:
	NavMeshPath() = default;
	NavMeshPath(std::vector<CU::Vector3f>&& aPath);

	const int GetSize() const;
	const bool Empty() const;

	CU::Vector3f operator[](int aPoint) const;
	const CU::Vector3f& operator[](int aPoint);
private:
	std::vector<CU::Vector3f> myPath;
};