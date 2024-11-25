#pragma once
#include "NavNode.h"
#include "NavPolygon.h"
#include "NavPortal.h"
#include "GameEngine/Intersections/AABB3D.hpp"
#include "GameEngine/Intersections/Ray.hpp"

#include "GameEngine/DebugDrawer/DebugLine.hpp"

class NavMeshPath;
class GameObject;

class NavMesh
{
public:
	void Init(std::vector<NavNode> aNavNodeList, std::vector<NavPolygon> aNavPolygonList, std::vector<NavPortal> aNavPortalList);
	void SetBoundingBox(CU::Vector3f aCenter, CU::Vector3f aExtents);
	const CU::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

	NavMeshPath FindPath(CU::Vector3f aStartingPos, CU::Vector3f aEndPos);
	void SnapGameObjectToNavMesh(GameObject& aGameObject);
	CU::Vector3f ClampToNavMesh(const CU::Vector3f& aPos) const;
	CU::Vector3f ClampToNearestEdge(const CU::Vector3f& aStart, const CU::Vector3f& aEnd) const;

	const bool RayCast(CU::Ray<float> aRay, CU::Vector3f& outHitPoint, bool aClampToNavMesh) const;

	void DrawDebugLines();
	void DrawBoundingBox();

private:
	const bool IsGoalInSameOrNeighbouringPolygon(CU::Vector3f aStartingPos, CU::Vector3f aEndPos) const;
	const bool IsGoalInSameOrNeighbouringPolygon(int aStartPolyIndex, int aEndPolyIndex, CU::Vector3f aEndPos) const;
	const int GetClosestNode(const CU::Vector3f& aPosition) const;
	const int GetClosestPolygon(const CU::Vector3f& aPosition) const;
	const CU::Vector3f GetClosestPointInNavMesh(const CU::Vector3f& aPosition) const;
	const bool IsPointInsidePolygon(NavPolygon aPolygon, CU::Vector3f aPosition) const;
	const bool NodesAreConnected(int aNodeIndexOne, int aNodeIndexTwo, int& inoutPortalIndex) const;

	std::vector<int> GetShortestNodePath(int aStartingNode, int aEndNode) const;
	std::vector<CU::Vector3f> ConvertPathIndexToWorldPos(std::vector<int> aPath);

	void ShortenEndNodes(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos, std::vector<CU::Vector3f>& inoutWorldPath);
	std::vector<CU::Vector3f> PathStraight(CU::Vector3f aStartingPos, CU::Vector3f aEndPos, const std::vector<int>& aNavNodePath) const;
	const bool CanPathStraight(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos) const;
	std::vector<CU::Vector3f> FunnelPath(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos, const std::vector<int>& aNavNodePath);

	std::vector<NavNode> myNodes;
	std::vector<NavPolygon> myPolygons;
	std::vector<NavPortal> myPortals;
	CU::AABB3D<float> myBoundingBox;

	struct AStarNode
	{
		float currentDistance = FLT_MAX;
		int predecessor = -1;
		bool hasBeenChecked = false;
	};
};