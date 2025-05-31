#include "NavMeshPath.h"

NavMeshPath::NavMeshPath(std::vector<Math::Vector3f>&& aPath)
{
	myPath = std::move(aPath);
}

const int NavMeshPath::GetSize() const
{
	return static_cast<int>(myPath.size());
}

const bool NavMeshPath::Empty() const
{
	return myPath.empty();
}

Math::Vector3f NavMeshPath::operator[](int aPoint) const
{
	assert(aPoint >= 0 && aPoint < myPath.size() && "Out of range!");
	return myPath[aPoint];
}

const Math::Vector3f& NavMeshPath::operator[](int aPoint)
{
	assert(aPoint >= 0 && aPoint < myPath.size() && "Out of range!");
	return myPath[aPoint];
}
