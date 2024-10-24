#include "NavMeshPath.h"

NavMeshPath::NavMeshPath(std::vector<CU::Vector3f> aPath)
{
	myPath = aPath;
}

const int NavMeshPath::GetSize() const
{
	return static_cast<int>(myPath.size());
}

const bool NavMeshPath::Empty() const
{
	return myPath.empty();
}

CU::Vector3f NavMeshPath::operator[](int aPoint) const
{
	assert(aPoint >= 0 && aPoint < myPath.size() && "Out of range!");
	return myPath[aPoint];
}

const CU::Vector3f& NavMeshPath::operator[](int aPoint)
{
	assert(aPoint >= 0 && aPoint < myPath.size() && "Out of range!");
	return myPath[aPoint];
}
