#include "Enginepch.h"
#include "NavMesh.h"
#include "NavMeshPath.h"

#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Intersections/Intersection3D.hpp"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

void NavMesh::Init(std::vector<NavNode> aNavNodeList, std::vector<NavPolygon> aNavPolygonList)
{
	myNodes = aNavNodeList;
	myPolygons = aNavPolygonList;
}

void NavMesh::SetBoundingBox(CU::Vector3f aCenter, CU::Vector3f aExtents)
{
	myBoundingBox.InitWithCenterAndExtents(aCenter, aExtents);
}

void NavMesh::DrawDebugLines()
{
	DebugDrawer& debugDrawer = Engine::GetInstance().GetDebugDrawer();

	for (auto& navNode : myNodes)
	{
		debugDrawer.DrawLine(navNode.position, navNode.position + CU::Vector3f(0, 20.0f, 0), { 1.0f, 1.0f, 0, 1.0f });

		for (auto& edge : navNode.edges)
		{
			debugDrawer.DrawLine(navNode.position + CU::Vector3f(0, 15.0f, 0), myNodes[edge].position + CU::Vector3f(0, 15.0f, 0), { 0, 0.6f, 1.0f, 1.0f });
		}
	}

	for (auto& navPolygon : myPolygons)
	{
		for (int i = 0; i < static_cast<int>(navPolygon.vertexPositions.size()); i++)
		{
			int nextPos = i + 1;
			if (nextPos >= navPolygon.vertexPositions.size())
			{
				nextPos = 0;
			}

			debugDrawer.DrawLine(navPolygon.vertexPositions[i] + CU::Vector3f(0, 10.0f, 0), navPolygon.vertexPositions[nextPos] + CU::Vector3f(0, 10.0f, 0), { 0.1f, 0.1f, 1.0f, 1.0f });
		}
	}
}

void NavMesh::DrawBoundingBox()
{
	Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(myBoundingBox, CU::Matrix4x4f(), { 1.0f, 1.0f, 1.0f, 1.0f });
}

void NavMesh::SnapGameObjectToNavMesh(GameObject& aGameObject)
{
	auto transform = aGameObject.GetComponent<Transform>();
	const int closestNode = GetClosestNode(transform->GetTranslation(true));
	transform->SetTranslation(myNodes[closestNode].position);
}

// Should be split into a step-function to allow for time-slicing & threading.
NavMeshPath NavMesh::FindPath(CU::Vector3f aStartingPos, CU::Vector3f aEndPos)
{
	int startIndex = GetClosestNode(aStartingPos);
	int endIndex = GetClosestNode(aEndPos);

	bool foundPath = false;
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> nodeHeap;

	nodeHeap.push(std::make_pair(0, startIndex));

	while (!nodeHeap.empty() || !foundPath)
	{
		if (nodeHeap.empty())
		{
			break;
		}

		int currentNodeIndex = nodeHeap.top().second;
		nodeHeap.pop();
		myNodes[currentNodeIndex].hasBeenChecked = true;
		myNodes[currentNodeIndex].currentDistance = 0;

		if (currentNodeIndex == endIndex)
		{
			foundPath = true;
		}

		for (int i = 0; i < static_cast<int>(myNodes[currentNodeIndex].edges.size()); i++)
		{
			const int& edgeWeight = myNodes[currentNodeIndex].edgeCosts[i];
			const int& neighbourNodeIndex = myNodes[currentNodeIndex].edges[i];

			if (myNodes[neighbourNodeIndex].hasBeenChecked || !myNodes[neighbourNodeIndex].isPassable)
			{
				continue;
			}

			if (myNodes[neighbourNodeIndex].currentDistance > myNodes[currentNodeIndex].currentDistance + edgeWeight)
			{
				myNodes[neighbourNodeIndex].currentDistance = myNodes[currentNodeIndex].currentDistance + edgeWeight;
				myNodes[neighbourNodeIndex].predecessor = currentNodeIndex;
				float remainingDistance = (myNodes[endIndex].position - myNodes[neighbourNodeIndex].position).LengthSqr();
				nodeHeap.push(std::make_pair(myNodes[neighbourNodeIndex].currentDistance + static_cast<int>(remainingDistance), neighbourNodeIndex));
			}
		}
	}

	std::vector<int> shortestPath;

	if (foundPath)
	{
		int pathNodeIndex = endIndex;

		while (myNodes[pathNodeIndex].predecessor >= 0)
		{
			shortestPath.push_back(pathNodeIndex);
			pathNodeIndex = myNodes[pathNodeIndex].predecessor;
		}

		shortestPath.push_back(startIndex);
	}

	std::reverse(shortestPath.begin(), shortestPath.end());

	for (auto& node : myNodes)
	{
		node.currentDistance = INT_MAX;
		node.predecessor = -1;
		node.hasBeenChecked = false;
	}
	
	std::vector<CU::Vector3f> worldPath = ConvertPathIndexToWorldPos(shortestPath);
	bool pathIsLongEnough = worldPath.size() >= 2;
	if (pathIsLongEnough)
	{
		CU::Vector3f lastNode = aEndPos - worldPath[worldPath.size() - 1];
		CU::Vector3f secondToLastNode = aEndPos - worldPath[worldPath.size() - 2];
		float dot = lastNode.GetNormalized().Dot(secondToLastNode.GetNormalized());
		bool pathContainsUnnecessaryNode = dot < 0;
		if (pathContainsUnnecessaryNode)
		{
			worldPath.back() = aEndPos;
		}
	}
	else
	{
		worldPath.emplace_back(aEndPos);
	}

	return NavMeshPath(worldPath);
}

const bool NavMesh::RayCast(CU::Ray<float> aRay, CU::Vector3f& outHitPoint) const
{
	if (!CU::IntersectionAABBRay<float>(myBoundingBox, aRay, outHitPoint))
	{
		return false;
	}

	// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
	for (auto& polygon : myPolygons)
	{
		CU::Plane<float> polygonPlane(polygon.vertexPositions[0], polygon.vertexPositions[1], polygon.vertexPositions[2]);

		CU::Vector3f polyIntersectionPoint;
		if (CU::IntersectionPlaneRay(polygonPlane, aRay, polyIntersectionPoint))
		{
			if (IsPointInsidePolygon(polygon, polyIntersectionPoint))
			{
				outHitPoint = polyIntersectionPoint;
				return true;
			}
		}
	}

	return false;
}

const int NavMesh::GetClosestNode(const CU::Vector3f& aPosition)
{
	// Needs to be optimized lol

	int nodeIndex = 0;
	float smallestDiff = FLT_MAX;

	for (int i = 0; i < static_cast<int>(myNodes.size()); i++)
	{
		if (!myNodes[i].isPassable) continue;

		float length = (myNodes[i].position - aPosition).LengthSqr();
		if (length < smallestDiff)
		{
			smallestDiff = length;
			nodeIndex = i;
		}
	}

	return nodeIndex;
}

std::vector<CU::Vector3f> NavMesh::ConvertPathIndexToWorldPos(std::vector<int> aPath)
{
	std::vector<CU::Vector3f> vector;

	for (auto& index : aPath)
	{
		vector.emplace_back(myNodes[index].position);
	}

	return vector;
}

const bool NavMesh::IsPointInsidePolygon(NavPolygon aPolygon, CU::Vector3f aPosition) const
{
	CU::Vector3f vertex0 = aPolygon.vertexPositions[0] - aPosition;
	CU::Vector3f vertex1 = aPolygon.vertexPositions[1] - aPosition;
	CU::Vector3f vertex2 = aPolygon.vertexPositions[2] - aPosition;

	CU::Vector3f u = vertex1.Cross(vertex2);
	CU::Vector3f v = vertex2.Cross(vertex0);
	CU::Vector3f w = vertex0.Cross(vertex1);

	if (u.Dot(v) < 0.0f)
	{
		return false;
	}

	if (u.Dot(w) < 0.0f)
	{
		return false;
	}

	return true;
}

std::vector<int> NavMesh::FunnelPath(const std::vector<int>& aNavNodePath)
{
	return aNavNodePath;
}
