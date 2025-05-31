#pragma once
#include "NavNode.h"
#include "NavPolygon.h"
#include "NavPortal.h"
#include "Math/AABB3D.hpp"
#include "Math/Ray.hpp"

#include "DebugDrawer/DebugLine.hpp"

class NavMeshPath;
class GameObject;

class NavMesh
{
public:
	void Init(std::vector<NavNode> aNavNodeList, std::vector<NavPolygon> aNavPolygonList, std::vector<NavPortal> aNavPortalList);
	void SetBoundingBox(Math::Vector3f aCenter, Math::Vector3f aExtents);
	const Math::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

	NavMeshPath FindPath(Math::Vector3f aStartingPos, Math::Vector3f aEndPos);
	void SnapGameObjectToNavMesh(GameObject& aGameObject);
	Math::Vector3f ClampToNavMesh(const Math::Vector3f& aPos) const;
	Math::Vector3f ClampToNearestEdge(const Math::Vector3f& aStart, const Math::Vector3f& aEnd) const;

	const bool RayCast(Math::Ray<float> aRay, Math::Vector3f& outHitPoint, bool aClampToNavMesh) const;

	void DrawDebugLines();
	void DrawBoundingBox();

private:
	const bool IsGoalInSameOrNeighbouringPolygon(Math::Vector3f aStartingPos, Math::Vector3f aEndPos) const;
	const bool IsGoalInSameOrNeighbouringPolygon(int aStartPolyIndex, int aEndPolyIndex, Math::Vector3f aEndPos) const;
	const int GetClosestNode(const Math::Vector3f& aPosition) const;
	const int GetClosestPolygon(const Math::Vector3f& aPosition) const;
	const Math::Vector3f GetClosestPointInNavMesh(const Math::Vector3f& aPosition) const;
	const bool IsPointInsidePolygon(NavPolygon aPolygon, Math::Vector3f aPosition) const;
	const bool NodesAreConnected(int aNodeIndexOne, int aNodeIndexTwo, int& inoutPortalIndex) const;

	std::vector<int> GetShortestNodePath(int aStartingNode, int aEndNode) const;
	std::vector<Math::Vector3f> ConvertPathIndexToWorldPos(std::vector<int> aPath);

	void ShortenEndNodes(const Math::Vector3f& aStartingPos, const Math::Vector3f& aEndPos, std::vector<Math::Vector3f>& inoutWorldPath);
	std::vector<Math::Vector3f> PathStraight(Math::Vector3f aStartingPos, Math::Vector3f aEndPos, const std::vector<int>& aNavNodePath) const;
	const bool CanPathStraight(const Math::Vector3f& aStartingPos, const Math::Vector3f& aEndPos) const;
	std::vector<Math::Vector3f> FunnelPath(const Math::Vector3f& aStartingPos, const Math::Vector3f& aEndPos, const std::vector<int>& aNavNodePath);

	std::vector<NavNode> myNodes;
	std::vector<NavPolygon> myPolygons;
	std::vector<NavPortal> myPortals;
	Math::AABB3D<float> myBoundingBox;

	struct AStarNode
	{
		float currentDistance = FLT_MAX;
		int predecessor = -1;
		bool hasBeenChecked = false;
	};
};