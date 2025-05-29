#include "Enginepch.h"
#include "NavMesh.h"
#include "NavMeshPath.h"

#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Intersections/Intersection3D.hpp"
#include "GameEngine/Intersections/Triangle.hpp"

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
	DebugDrawer& debugDrawer = Engine::Get().GetDebugDrawer();
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
	Engine::Get().GetDebugDrawer().DrawBoundingBox(myBoundingBox, CU::Matrix4x4f(), { 1.0f, 1.0f, 1.0f, 1.0f });
}

void NavMesh::SnapGameObjectToNavMesh(GameObject& aGameObject)
{
	auto transform = aGameObject.GetComponent<Transform>();
	const int closestNode = GetClosestNode(transform->GetTranslation(true));
	transform->SetTranslation(myNodes[closestNode].position);
}

CU::Vector3f NavMesh::ClampToNavMesh(const CU::Vector3f& aPos) const
{
	for (int i = 0; i < (int)myPolygons.size(); ++i)
	{
		const NavPolygon& polygon = myPolygons[i];

		CU::Plane<float> plane;
		plane.InitWith3Points(
			polygon.vertexPositions[0],
			polygon.vertexPositions[1],
			polygon.vertexPositions[2]);

		// essentially we offset ray 50 cm upwards, and then have a threshold of 100cm of snapping to closest node downwards

		CU::Vector3f posOffset = aPos + CU::Vector3f(0.0f, 50.0f, 0.0);
		CU::Vector3f polyIntersectionPoint;
		bool polyIntersection = CU::IntersectionPlaneRay(plane, CU::Ray<float>(posOffset, CU::Vector3f(0, -1.0f, 0)), polyIntersectionPoint);

		if (polyIntersection &&  IsPointInsidePolygon(myPolygons[i], polyIntersectionPoint))
		{
			return polyIntersectionPoint;
		}
	}

	CU::Vector3f closestPoint = aPos;
	float closestPointDist = FLT_MAX;

	for (int i = 0; i < (int)myPolygons.size(); ++i)
	{
		const NavPolygon& polygon = myPolygons[i];

		for (int vertexIndex = 0; vertexIndex < polygon.vertexPositions.size(); ++vertexIndex)
		{
			int nextIndex = vertexIndex + 1 >= polygon.vertexPositions.size() ? 0 : vertexIndex + 1;
			const CU::Vector3f& vertexPos1 = polygon.vertexPositions[vertexIndex];
			const CU::Vector3f& vertexPos2 = polygon.vertexPositions[nextIndex];

			const auto point = CU::Vector3f::ClosestPointOnSegment(vertexPos1, vertexPos2, aPos);
			const float dist = (aPos - point).LengthSqr();

			if (dist < closestPointDist)
			{
				closestPoint = point;
				closestPointDist = dist;
			}
		}
	}

	return closestPoint;
}

CU::Vector3f NavMesh::ClampToNearestEdge(const CU::Vector3f& aStart, const CU::Vector3f& aEnd) const
{
	CU::Vector3f closestPoint;
	float closestPointDist = FLT_MAX;

	for (int i = 0; i < (int)myPolygons.size(); ++i)
	{
		const NavPolygon& polygon = myPolygons[i];

		for (int vertexIndex = 0; vertexIndex < polygon.vertexPositions.size(); ++vertexIndex)
		{
			int nextIndex = vertexIndex + 1 >= polygon.vertexPositions.size() ? 0 : vertexIndex + 1;
			const CU::Vector3f& vertexPos1 = polygon.vertexPositions[vertexIndex];
			const CU::Vector3f& vertexPos2 = polygon.vertexPositions[nextIndex];

			auto closestPoints = CU::Vector3f::ClosestPointsSegmentSegment(aStart, aEnd, vertexPos1, vertexPos2);

			CU::Vector3f point1 = std::get<0>(closestPoints);
			CU::Vector3f point2 = std::get<1>(closestPoints);
			const float dist = (point1 - point2).LengthSqr();

			if (dist < closestPointDist)
			{
				closestPoint = point2;
				closestPointDist = dist;
			}
		}
	}

	return closestPoint;
}

// Should be split into a step-function to allow for time-slicing & threading.
NavMeshPath NavMesh::FindPath(CU::Vector3f aStartingPos, CU::Vector3f aEndPos)
{
	std::vector<CU::Vector3f> worldPath;

	if (CanPathStraight(aStartingPos, aEndPos))
	{
		worldPath.emplace_back(ClampToNavMesh(aStartingPos));
		worldPath.emplace_back(ClampToNavMesh(aEndPos));
		return NavMeshPath(std::move(worldPath));
	}

	int startIndex = GetClosestPolygon(aStartingPos); // can be on own thread
	int endIndex = GetClosestPolygon(aEndPos); // can be on own thread

	// Wait for results before executing
	std::vector<int> shortestNodePath = GetShortestNodePath(startIndex, endIndex);

	if (shortestNodePath.empty())
	{
		return NavMeshPath();
	}

	worldPath = FunnelPath(aStartingPos, aEndPos, shortestNodePath);
	if (!worldPath.empty())
	{
		return NavMeshPath(std::move(worldPath));
	}

	return NavMeshPath();
}

const bool NavMesh::RayCast(CU::Ray<float> aRay, CU::Vector3f& outHitPoint, bool aClampToNavMesh) const
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
		if (polyIntersection && IsPointInsidePolygon(polygon, polyIntersectionPoint))
		{
			if (intersectionDistance < closestPolygon)
			{
				outHitPoint = polyIntersectionPoint;
				closestPolygon = intersectionDistance;
				hitNavMesh = true;
			}
		}
	}

	if (!hitNavMesh && aClampToNavMesh)
	{
		outHitPoint = ClampToNearestEdge(aRay.GetOrigin(), aRay.GetOrigin() + aRay.GetDirection() * 10000.0f);
		hitNavMesh = true;
	}

	return hitNavMesh;
}

const bool NavMesh::IsGoalInSameOrNeighbouringPolygon(CU::Vector3f aStartingPos, CU::Vector3f aEndPos) const
{
	int startingNodeIndex = GetClosestPolygon(aStartingPos);
	if (IsPointInsidePolygon(myPolygons[startingNodeIndex], aEndPos)) return true;

	int goalNodeIndex = GetClosestPolygon(aEndPos);
	if (startingNodeIndex == goalNodeIndex)
		return true;

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

const bool NavMesh::IsGoalInSameOrNeighbouringPolygon(int aStartPolyIndex, int aEndPolyIndex, CU::Vector3f aEndPos) const
{
	if (IsPointInsidePolygon(myPolygons[aStartPolyIndex], aEndPos))
		return true;

	if (aStartPolyIndex == aEndPolyIndex)
		return true;

	bool hasConnection = false;
	for (auto& portalIndex : myNodes[aStartPolyIndex].portals)
	{
		if (myPortals[portalIndex].GetOtherNode(aStartPolyIndex) == aEndPolyIndex)
		{
			hasConnection = true;
			break;
		}
	}

	return hasConnection;
}

std::vector<int> NavMesh::GetShortestNodePath(int aStartingNode, int aEndNode) const
{
	std::vector<AStarNode> astarNodes;
	astarNodes.resize(myNodes.size());

	int startIndex = aStartingNode;
	int endIndex = aEndNode;

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

		AStarNode& pathNode = astarNodes[currentNodeIndex];

		if (pathNode.hasBeenChecked)
			continue;

		pathNode.hasBeenChecked = true;
		pathNode.currentDistance = 0;

		if (currentNodeIndex == endIndex)
		{
			foundPath = true;
			continue;
		}

		for (int i = 0; i < static_cast<int>(myNodes[currentNodeIndex].portals.size()); i++)
		{
			const NavPortal& portal = myPortals[myNodes[currentNodeIndex].portals[i]];

			float edgeWeight = portal.cost;
			int neighbourNodeIndex = portal.GetOtherNode(currentNodeIndex);

			if (!myNodes[neighbourNodeIndex].isPassable)
			{
				continue;
			}

			AStarNode& neighbourPathNode = astarNodes[neighbourNodeIndex];

			float gScore = pathNode.currentDistance + edgeWeight;
			if (gScore < neighbourPathNode.currentDistance)
			{
				neighbourPathNode.currentDistance = gScore;
				neighbourPathNode.predecessor = currentNodeIndex;

				float remainingDistance = (myNodes[endIndex].position - myNodes[neighbourNodeIndex].position).Length(); // hScore

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

const CU::Vector3f NavMesh::GetClosestPointInNavMesh(const CU::Vector3f& aPosition) const
{
	int closestNodeIndex = GetClosestNode(aPosition);
	const NavPolygon& closestPoly = myPolygons[closestNodeIndex];
	CU::Triangle<float> tri(closestPoly.vertexPositions[0], closestPoly.vertexPositions[1], closestPoly.vertexPositions[2]);

	CU::Vector3f closestPoint = tri.ClosestPointOnTriangle(aPosition);

	return closestPoint;
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

std::vector<CU::Vector3f> NavMesh::PathStraight(CU::Vector3f aStartingPos, CU::Vector3f aEndPos, const std::vector<int>& aNavNodePath) const
{
	std::vector<CU::Vector3f> result;

	CU::Vector3f startPos = aStartingPos;
	CU::Vector3f endPos = aEndPos;

	startPos.y = 0.0f;
	endPos.y = 0.0f;

	std::vector<std::array<CU::Vector3f, 2>> intersectedEdges;

	const auto FindIntersections =
		[&intersectedEdges, startPos, endPos](const NavPolygon& aPolygon)
		{
			for (int vertexIndex = 0; vertexIndex < aPolygon.vertexPositions.size(); ++vertexIndex)
			{
				int nextIndex = (vertexIndex + 1) >= aPolygon.vertexPositions.size() ? 0 : vertexIndex + 1;

				CU::Vector3f vertexPos1 = aPolygon.vertexPositions[vertexIndex];
				CU::Vector3f vertexPos2 = aPolygon.vertexPositions[nextIndex];

				CU::Vector3f v0 = vertexPos1;
				CU::Vector3f v1 = vertexPos2;

				v0.y = 0.0f;
				v1.y = 0.0f;

				auto closestPoints = CU::Vector3f::ClosestPointsSegmentSegment(startPos, endPos, v0, v1);

				CU::Vector3f point1 = std::get<0>(closestPoints);
				CU::Vector3f point2 = std::get<1>(closestPoints);

				if (point1 == point2)
				{
					intersectedEdges.push_back({ vertexPos1, vertexPos2 });
				}

			}
		};

	for (const auto& nodeIndex : aNavNodePath)
	{
		const NavPolygon& polygon = myPolygons[nodeIndex];
		const NavNode& node = myNodes[nodeIndex];

		FindIntersections(polygon);

		for (auto& portalIndex : node.portals)
		{
			const NavPortal& portal = myPortals[portalIndex];
			int neighbourNodeIndex = portal.GetOtherNode(nodeIndex);

			const NavPolygon& neighbourPolygon = myPolygons[neighbourNodeIndex];

			FindIntersections(neighbourPolygon);
		}
	}

	if (intersectedEdges.empty())
		return result;

	int intersectedPortals = 0;
	for (auto& edge : intersectedEdges)
	{
		for (auto& portal : myPortals)
		{
			if (portal.IsSameEdge(edge[0], edge[1]))
			{
				intersectedPortals++;
				break;
			}
		}
	}

	if (intersectedPortals == intersectedEdges.size())
	{
		result.emplace_back(ClampToNavMesh(aStartingPos));

		std::vector<CU::Vector3f> pathPoints;
		pathPoints.reserve(intersectedEdges.size() + 2);

		pathPoints.emplace_back(aStartingPos);

		for (int i = 0; i < intersectedEdges.size(); ++i)
		{
			const auto& edge = intersectedEdges[i];

			auto closestPoints = CU::Vector3f::ClosestPointsSegmentSegment(aStartingPos, aEndPos, edge[0], edge[1]);
			CU::Vector3f point2 = std::get<1>(closestPoints);

			pathPoints.emplace_back(point2);
		}

		pathPoints.emplace_back(aEndPos);

		std::sort(pathPoints.begin(), pathPoints.end(),
			[aStartingPos](const auto& aLeft, const auto& aRight)
			{
				const float leftDistToStart = CU::Vector3f::DistanceSqr(aStartingPos, aLeft);
				const float rightDistToStart = CU::Vector3f::DistanceSqr(aStartingPos, aRight);

				return leftDistToStart < rightDistToStart;
			});

		CU::Vector3f prevPoint = pathPoints.front();

		for (int i = 1; i < pathPoints.size() - 1; ++i)
		{
			const CU::Vector3f currPoint = pathPoints[i];
			const CU::Vector3f nextPoint = pathPoints[i + 1];

			float distSqr = CU::Vector3f::DistanceSqrToLine(prevPoint, nextPoint, currPoint);

			static constexpr float tolerance = 1.0f;
			if (distSqr > tolerance)
			{
				prevPoint = currPoint;
				result.emplace_back(ClampToNavMesh(currPoint));
			}
		}

		result.emplace_back(ClampToNavMesh(pathPoints.back()));

		return result;
	}

	return result;
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

	if (intersectedEdges.empty()) return false;

	int intersectedPortals = 0;
	for (auto& edge : intersectedEdges)
	{
		for (auto& portal : myPortals)
		{
			if (portal.IsSameEdge(edge[0], edge[1]))
			{
				intersectedPortals++;
				break;
			}
		}
	}

	return intersectedPortals == intersectedEdges.size();
}

std::vector<CU::Vector3f> NavMesh::FunnelPath(const CU::Vector3f& aStartingPos, const CU::Vector3f& aEndPos, const std::vector<int>& aNavNodePath)
{
	if (aNavNodePath.size() <= 2)
		return { aEndPos };

	const auto Vec2Right =
		[](const CU::Vector3f& aLeft, const CU::Vector3f& aRight) -> float
		{
			return (CU::Vector2f(-aLeft.z, aLeft.x).GetNormalized()
				.Dot(CU::Vector2f(aRight.x, aRight.z).GetNormalized())) > 0.0f ? 1.0f : -1.0f;
		};

	std::vector<CU::Vector3f> path{};
	std::vector<CU::Vector3f> leftVertices;
	std::vector<CU::Vector3f> rightVertices;

	leftVertices.resize(aNavNodePath.size() + 1);
	rightVertices.resize(aNavNodePath.size() + 1);

	for (int i = 0; i < (int)aNavNodePath.size() - 1; ++i) // find active portals
	{
		const int pathNodeIndex = aNavNodePath[i];
		const int neighbourIndex = aNavNodePath[i + 1];

		const NavNode& node = myNodes[pathNodeIndex];

		for (int j = 0; j < node.portals.size(); ++j)
		{
			const NavPortal& portal = myPortals[node.portals[j]];

			if (portal.GetOtherNode(pathNodeIndex) == neighbourIndex)
			{
				const NavNode& connectionNode = myNodes[neighbourIndex];

				const CU::Vector3f leftVertex = portal.vertices[0];
				const CU::Vector3f rightVertex = portal.vertices[1];

				const CU::Vector3f dir = connectionNode.position - node.position;

				if (Vec2Right(dir, leftVertex - node.position) < 0.0f)
				{
					leftVertices[i + 1] = rightVertex;
					rightVertices[i + 1] = leftVertex;
				}
				else
				{
					leftVertices[i + 1] = leftVertex;
					rightVertices[i + 1] = rightVertex;
				}

				break;
			}
		}
	}

	leftVertices[0] = aStartingPos;
	rightVertices[0] = aStartingPos;

	leftVertices[aNavNodePath.size()] = aEndPos;
	rightVertices[aNavNodePath.size()] = aEndPos;

	CU::Vector3f	apex = aStartingPos;
	int				left = 1;
	int				right = 1;

	for (int i = 2; i <= (int)aNavNodePath.size() && path.size() < aNavNodePath.size(); ++i)
	{
		if (!CU::Vector3f::Equal(leftVertices[i], leftVertices[left]) && i > left)
		{
			CU::Vector3f newSide = leftVertices[i] - apex;

			// If new side does not widen funnel, update.
			if (Vec2Right(newSide, leftVertices[left] - apex) > 0.0f && !CU::Vector3f::Equal(apex, leftVertices[left], 0.00001f))
			{
				// If new side crosses other side, update apex.
				if (Vec2Right(newSide, rightVertices[right] - apex) > 0.0f)
				{
					// Find next vertex.
					int next = right;
					for (int j = next; j <= (int)aNavNodePath.size(); j++)
					{
						if (!CU::Vector3f::Equal(rightVertices[j], rightVertices[next]))
						{
							next = j;
							break;
						}
					}

					i = right;

					path.emplace_back(ClampToNavMesh(rightVertices[right]));
					apex = rightVertices[right];
					right = next;

					continue;
				}
				else
				{
					left = i;
				}
			}
		}

		if (!CU::Vector3f::Equal(rightVertices[i], rightVertices[right]) && i > right)
		{
			CU::Vector3f newSide = rightVertices[i] - apex;

			if (Vec2Right(newSide, rightVertices[right] - apex) < 0.0f && !CU::Vector3f::Equal(apex, rightVertices[right], 0.00001f))
			{
				if (Vec2Right(newSide, leftVertices[left] - apex) < 0.0f)
				{
					int next = left;
					for (int j = next; j <= (int)aNavNodePath.size(); j++)
					{
						if (!CU::Vector3f::Equal(leftVertices[j], leftVertices[next]))
						{
							next = j;
							break;
						}
					}

					i = left;

					path.emplace_back(ClampToNavMesh(leftVertices[left]));
					apex = leftVertices[left];
					left = next;

					continue;
				}
				else
				{
					right = i;
				}
			}
		}
	}

	path.emplace_back(ClampToNavMesh(aEndPos));

	return path;
}