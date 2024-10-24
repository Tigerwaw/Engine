#pragma once
#include "NavNode.h"
#include "NavPolygon.h"
#include "NavPortal.h"
#include "GameEngine/Intersections/AABB3D.hpp"
#include "GameEngine/Intersections/Ray.hpp"

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

	const bool RayCast(CU::Ray<float> aRay, CU::Vector3f& outHitPoint) const;

	void DrawDebugLines();
	void DrawBoundingBox();

private:
	const bool CanPathStraight(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos) const;
	std::vector<int> GetShortestNodePath(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos);

	const int GetClosestNode(const CU::Vector3f& aPosition);
	std::vector<CU::Vector3f> ConvertPathIndexToWorldPos(std::vector<int> aPath);

	const bool IsPointInsidePolygon(NavPolygon aPolygon, CU::Vector3f aPosition) const;

	void ShortenEndNodes(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos, std::vector<CU::Vector3f>& inoutWorldPath);
	std::vector<CU::Vector3f> FunnelPath(const std::vector<int>& aNavNodePath);

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