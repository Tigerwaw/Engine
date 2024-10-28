#include "Enginepch.h"
#include "NavMesh.h"
#include "NavMeshPath.h"

#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Intersections/Intersection3D.hpp"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

void NavMesh::Init(std::vector<NavNode> aNavNodeList, std::vector<NavPolygon> aNavPolygonList, std::vector<NavPortal> aNavPortalList)
{
	myNodes = aNavNodeList;
	myPolygons = aNavPolygonList;
	myPortals = aNavPortalList;
}

void NavMesh::SetBoundingBox(CU::Vector3f aCenter, CU::Vector3f aExtents)
{
	myBoundingBox.InitWithCenterAndExtents(aCenter, aExtents);
}

void NavMesh::DrawDebugLines()
{
	DebugDrawer& debugDrawer = Engine::GetInstance().GetDebugDrawer();
	CU::Vector3f debugEdgeOffset = { 0, 10.0f, 0 };
	CU::Vector3f debugConnectionOffset = { 0, 15.0f, 0 };

	int nodeIndex = 0;
	for (auto& navNode : myNodes)
	{
		debugDrawer.DrawLine(navNode.position, navNode.position + debugConnectionOffset, { 1.0f, 1.0f, 0, 1.0f });

		for (auto& edge : navNode.portals)
		{
			debugDrawer.DrawLine(navNode.position + debugConnectionOffset, myNodes[myPortals[edge].GetOtherNode(nodeIndex)].position + debugConnectionOffset, { 0, 0.6f, 1.0f, 1.0f });
		}

		nodeIndex++;
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

			debugDrawer.DrawLine(navPolygon.vertexPositions[i] + debugEdgeOffset, navPolygon.vertexPositions[nextPos] + debugEdgeOffset, { 0.1f, 0.1f, 1.0f, 1.0f });
		}
	}
}

void NavMesh::DrawBoundingBox()
{
	Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(myBoundingBox, CU::Matrix4x4f(), { 1.0f, 1.0f, 1.0f, 1.0f });
}

void NavMesh::DrawFunnelLines()
{
	for (auto& funnelLine : myFunnelLines)
	{
		Engine::GetInstance().GetDebugDrawer().DrawLine(funnelLine);
	}
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
	if (CanPathStraight(aStartingPos, aEndPos))
	{
		std::vector<CU::Vector3f> newPath;
		newPath.emplace_back(aStartingPos);
		newPath.emplace_back(aEndPos);
		return NavMeshPath(newPath);
	}

	std::vector<int> shortestNodePath = GetShortestNodePath(aStartingPos, aEndPos);
	//std::vector<CU::Vector3f> worldPath = ConvertPathIndexToWorldPos(shortestNodePath);
	//ShortenEndNodes(aStartingPos, aEndPos, worldPath);
	std::vector<CU::Vector3f> funneledPath = FunnelPath(aStartingPos, aEndPos, shortestNodePath);

	return NavMeshPath(funneledPath);
}

const bool NavMesh::RayCast(CU::Ray<float> aRay, CU::Vector3f& outHitPoint) const
{
	if (!CU::IntersectionAABBRay<float>(myBoundingBox, aRay, outHitPoint))
	{
		if (!myBoundingBox.IsInside(aRay.GetOrigin()))
		{
			return false;
		}
	}

	// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
	float closestPolygon = FLT_MAX;
	bool hitNavMesh = false;
	for (auto& polygon : myPolygons)
	{
		CU::Plane<float> polygonPlane(polygon.vertexPositions[0], polygon.vertexPositions[1], polygon.vertexPositions[2]);

		CU::Vector3f polyIntersectionPoint;
		bool polyIntersection = CU::IntersectionPlaneRay(polygonPlane, aRay, polyIntersectionPoint);
		float intersectionDistance = polyIntersectionPoint.LengthSqr();
		if (polyIntersection && intersectionDistance < closestPolygon)
		{
			if (IsPointInsidePolygon(polygon, polyIntersectionPoint))
			{
				outHitPoint = polyIntersectionPoint;
				closestPolygon = intersectionDistance;
				hitNavMesh = true;
			}
		}
	}

	return hitNavMesh;
}

const bool NavMesh::IsGoalInSameOrNeighbouringPolygon(CU::Vector3f aStartingPos, CU::Vector3f aEndPos) const
{
	int startingNodeIndex = GetClosestPolygon(aStartingPos);
	if (IsPointInsidePolygon(myPolygons[startingNodeIndex], aEndPos)) return true;

	int goalNodeIndex = GetClosestPolygon(aEndPos);
	bool hasConnection = false;
	for (auto& portalIndex : myNodes[startingNodeIndex].portals)
	{
		if (myPortals[portalIndex].GetOtherNode(startingNodeIndex) == goalNodeIndex)
		{
			hasConnection = true;
			break;
		}
	}

	return hasConnection;
}

const bool NavMesh::CanPathStraight(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos) const
{
	if (IsGoalInSameOrNeighbouringPolygon(aStartingPos, aEndPos)) return true;

	CU::Vector3f direction = (aEndPos - aStartingPos).GetNormalized();
	float dot = direction.Dot(CU::Vector3f(0, 1.0f, 0));
	bool rayIsNotAtASteepAngle = dot > -0.25f && dot < 0.25f;
	if (!rayIsNotAtASteepAngle) return false;

	std::vector<std::array<CU::Vector3f, 2>> intersectedEdges;

	for (auto& polygon : myPolygons)
	{
		for (int vertexIndex = 0; vertexIndex < polygon.vertexPositions.size(); ++vertexIndex)
		{
			int nextIndex = vertexIndex + 1 >= polygon.vertexPositions.size() ? 0 : vertexIndex + 1;
			const CU::Vector3f& vertexPos1 = polygon.vertexPositions[vertexIndex];
			const CU::Vector3f& vertexPos2 = polygon.vertexPositions[nextIndex];
			auto closestPoints = CU::Vector3f::ClosestPointsSegmentSegment(aStartingPos, aEndPos, vertexPos1, vertexPos2);
			float tolerance = 10.0f;
			CU::Vector3f point1 = std::get<0>(closestPoints);
			CU::Vector3f point2 = std::get<1>(closestPoints);
			if ((point1 - point2).LengthSqr() < tolerance)
			{
				intersectedEdges.push_back({ vertexPos1, vertexPos2 });
			}
		}
	}

	for (auto& edge : intersectedEdges)
	{
		bool isPortal = false;
		for (auto& portal : myPortals)
		{
			if (portal.IsSameEdge(edge[0], edge[1]))
			{
				isPortal = true;
			}
		}

		if (!isPortal)
		{
			return false;
		}
	}

	return true;
}

std::vector<int> NavMesh::GetShortestNodePath(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos)
{
	std::vector<AStarNode> astarNodes;
	astarNodes.resize(myNodes.size());

	int startIndex = GetClosestNode(aStartingPos);
	int endIndex = GetClosestNode(aEndPos);

	bool foundPath = false;
	std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> nodeHeap;

	nodeHeap.push(std::make_pair(0.0f, startIndex));

	while (!nodeHeap.empty() || !foundPath)
	{
		if (nodeHeap.empty())
		{
			break;
		}

		int currentNodeIndex = nodeHeap.top().second;
		nodeHeap.pop();
		astarNodes[currentNodeIndex].hasBeenChecked = true;
		astarNodes[currentNodeIndex].currentDistance = 0;

		if (currentNodeIndex == endIndex)
		{
			foundPath = true;
		}

		for (int i = 0; i < static_cast<int>(myNodes[currentNodeIndex].portals.size()); i++)
		{
			NavPortal& portal = myPortals[myNodes[currentNodeIndex].portals[i]];
			const float& edgeWeight = portal.cost;
			const int& neighbourNodeIndex = portal.GetOtherNode(currentNodeIndex);

			if (astarNodes[neighbourNodeIndex].hasBeenChecked || !myNodes[neighbourNodeIndex].isPassable)
			{
				continue;
			}

			if (astarNodes[neighbourNodeIndex].currentDistance > astarNodes[currentNodeIndex].currentDistance + edgeWeight)
			{
				astarNodes[neighbourNodeIndex].currentDistance = astarNodes[currentNodeIndex].currentDistance + edgeWeight;
				astarNodes[neighbourNodeIndex].predecessor = currentNodeIndex;
				float remainingDistance = (myNodes[endIndex].position - myNodes[neighbourNodeIndex].position).LengthSqr();
				nodeHeap.push(std::make_pair(astarNodes[neighbourNodeIndex].currentDistance + remainingDistance, neighbourNodeIndex));
			}
		}
	}

	std::vector<int> shortestPath;

	if (foundPath)
	{
		int pathNodeIndex = endIndex;

		while (astarNodes[pathNodeIndex].predecessor >= 0)
		{
			shortestPath.push_back(pathNodeIndex);
			pathNodeIndex = astarNodes[pathNodeIndex].predecessor;
		}

		shortestPath.push_back(startIndex);
	}

	std::reverse(shortestPath.begin(), shortestPath.end());

	return shortestPath;
}

const int NavMesh::GetClosestNode(const CU::Vector3f& aPosition) const
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

const int NavMesh::GetClosestPolygon(const CU::Vector3f& aPosition) const
{
	int polyIndex = 0;
	for (auto& poly : myPolygons)
	{
		if (IsPointInsidePolygon(poly, aPosition)) return polyIndex;
		polyIndex++;
	}

	return GetClosestNode(aPosition);
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

const bool NavMesh::NodesAreConnected(int aNodeIndexOne, int aNodeIndexTwo, int& inoutPortalIndex) const
{
	for (const auto& portal : myNodes[aNodeIndexOne].portals)
	{
		if (myPortals[portal].GetOtherNode(aNodeIndexOne) == aNodeIndexTwo)
		{
			inoutPortalIndex = portal;
			return true;
		}
	}

	return false;
}

void NavMesh::ShortenEndNodes(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos, std::vector<CU::Vector3f>& inoutWorldPath)
{
	if (inoutWorldPath.empty()) return;

	bool addEndPos = true;
	if (inoutWorldPath.size() >= 2)
	{
		float startToFirstNode = (aStartingPos - inoutWorldPath[0]).LengthSqr();
		float startToSecondNode = (aStartingPos - inoutWorldPath[1]).LengthSqr();
		float firstToSecondNode = (inoutWorldPath[0] - inoutWorldPath[1]).LengthSqr();
		if (startToFirstNode + firstToSecondNode > startToSecondNode)
		{
			inoutWorldPath.erase(inoutWorldPath.begin());
		}
	}

	if (inoutWorldPath.size() >= 2)
	{
		CU::Vector3f lastNode = inoutWorldPath[inoutWorldPath.size() - 1];
		CU::Vector3f secondLastNode = inoutWorldPath[inoutWorldPath.size() - 2];
		float endToLastNode = (aEndPos - lastNode).LengthSqr();
		float endToSecondLastNode = (aEndPos - secondLastNode).LengthSqr();
		float lastNodeToSecondLastNode = (secondLastNode - lastNode).LengthSqr();
		if (endToLastNode + lastNodeToSecondLastNode > endToSecondLastNode)
		{
			inoutWorldPath.back() = aEndPos;
			addEndPos = false;
		}
	}

	if (addEndPos)
	{
		inoutWorldPath.emplace_back(aEndPos);
	}
}

std::vector<CU::Vector3f> NavMesh::FunnelPath(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos, const std::vector<int>& aNavNodePath)
{
	aStartingPos;
	aEndPos;
	if (aNavNodePath.empty()) return std::vector<CU::Vector3f>();
	std::vector<CU::Vector3f> left;
	std::vector<CU::Vector3f> right;

	for (int pathNodeIndex = 1; pathNodeIndex < aNavNodePath.size() - 1; pathNodeIndex++)
	{
		int currentNodeIndex = aNavNodePath[pathNodeIndex];
		int nextNodeIndex = aNavNodePath[pathNodeIndex + 1];

		int connectedPortalIndex;
		if (!NodesAreConnected(currentNodeIndex, nextNodeIndex, connectedPortalIndex))
		{
			return std::vector<CU::Vector3f>();
		}
		
		const NavNode& currentNode = myNodes[currentNodeIndex];
		const NavNode& nextNode = myNodes[nextNodeIndex];
		const NavPolygon& currentPolygon = myPolygons[currentNodeIndex];
		for (const auto& polyVertexPos : currentPolygon.vertexPositions)
		{
			CU::Vector3f directionToNextNode = (nextNode.position - currentNode.position).GetNormalized();
			CU::Vector3f directionToVertex = (polyVertexPos - currentNode.position).GetNormalized();
			if (directionToNextNode.Dot(directionToNextNode) < 0) continue;

			bool leftContainsVertex = std::find(left.begin(), left.end(), polyVertexPos) != left.end();
			bool rightContainsVertex = std::find(right.begin(), right.end(), polyVertexPos) != right.end();
			if (!leftContainsVertex && !rightContainsVertex)
			{
				auto* vectorPtr = &left;
				if (directionToNextNode.Cross({ 1.0f, 0, 0 }).Dot(directionToVertex) > 0)
				{
					vectorPtr = &right;
				}

				vectorPtr->emplace_back(polyVertexPos);
			}
		}
	}

	for (int leftIndex = 1; leftIndex < left.size(); leftIndex++)
	{
		CU::Vector3f offset = { 0, 5.0f, 0 };
		DebugLine pathLine;
		pathLine.Color = { 1.0f, 1.0f, 0.0f, 1.0f };
		CU::Vector3f previousPos = left[leftIndex - 1];
		pathLine.From = previousPos + offset;
		pathLine.To = left[leftIndex] + offset;
		myFunnelLines.emplace_back(pathLine);
	}

	for (int rightIndex = 1; rightIndex < right.size(); rightIndex++)
	{
		CU::Vector3f offset = { 0, 5.0f, 0 };
		DebugLine pathLine;
		pathLine.Color = { 1.0f, 0.0f, 1.0f, 1.0f };
		CU::Vector3f previousPos = right[rightIndex - 1];
		pathLine.From = previousPos + offset;
		pathLine.To = right[rightIndex] + offset;
		myFunnelLines.emplace_back(pathLine);
	}

	std::vector<CU::Vector3f> funneledPath;
	return funneledPath;
}
