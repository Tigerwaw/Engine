#include <vector>
#include <queue>
#include <algorithm>
#include "NavVertex.h"

int MapWidth = 20;
int MapHeight = 20;
int TileCount = MapWidth * MapHeight;

enum class Tile
{
	Impassable,
	Passable
};

inline std::vector<int> GetNeighbourIndices(int aVertexIndex)
{
	std::vector<int> neighbours;

	int leftNeighbour = aVertexIndex - 1;
	if (leftNeighbour % MapWidth >= 0 && leftNeighbour % MapWidth < MapWidth - 1)
	{
		neighbours.push_back(leftNeighbour);
	}

	int rightNeighbour = aVertexIndex + 1;
	if (rightNeighbour % MapWidth > 0)
	{
		neighbours.push_back(rightNeighbour);
	}

	int topNeighbour = aVertexIndex - MapWidth;
	if (topNeighbour >= 0)
	{
		neighbours.push_back(topNeighbour);
	}

	int bottomNeighbour = aVertexIndex + MapWidth;
	if (bottomNeighbour < TileCount)
	{
		neighbours.push_back(bottomNeighbour);
	}

	return neighbours;
}

inline int GetRemainingDistance(int aStartIndex, int anEndIndex)
{
	int startColumn = aStartIndex % MapWidth;
	int startRow = aStartIndex / MapHeight;
	int endColumn = anEndIndex % MapWidth;
	int endRow = anEndIndex / MapHeight;

	int rowDiff = abs(endColumn - startColumn);
	int columnDiff = abs(endRow - startRow);
	int heuristicDistance = columnDiff + rowDiff;
	return heuristicDistance;
}

inline std::vector<Vertex> CreateVertexVector(const std::vector<Tile>& aMap, int aStartIndex)
{
	std::vector<Vertex> vertexVector;

	for (int i = 0; i < aMap.size(); i++)
	{
		Vertex newVertex = Vertex();

		if (i == aStartIndex)
		{
			newVertex.currentDistance = 0;
		}

		std::vector<int> neighbours = GetNeighbourIndices(i);
		for (auto& neighbour : neighbours)
		{
			newVertex.edges.emplace(1, neighbour);
		}

		vertexVector.push_back(newVertex);
	}

	return vertexVector;
}

inline std::vector<int> AStar(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex)
{
	bool foundPath = false;
	std::vector<Vertex> vertices = CreateVertexVector(aMap, aStartIndex);
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> vertexHeap;

	vertexHeap.push(std::make_pair(0, aStartIndex));

	while (!vertexHeap.empty() || !foundPath)
	{
		if (vertexHeap.empty())
		{
			break;
		}

		int currentVertexIndex = vertexHeap.top().second;
		vertexHeap.pop();
		vertices[currentVertexIndex].hasBeenChecked = true;

		if (currentVertexIndex == anEndIndex)
		{
			foundPath = true;
		}

		for (auto& edge : vertices[currentVertexIndex].edges)
		{
			const int& edgeWeight = edge.first;
			const int& neighbourVertexIndex = edge.second;

			if (vertices[neighbourVertexIndex].hasBeenChecked || aMap[neighbourVertexIndex] == Tile::Impassable)
			{
				continue;
			}

			if (vertices[neighbourVertexIndex].currentDistance > vertices[currentVertexIndex].currentDistance + edgeWeight)
			{
				vertices[neighbourVertexIndex].currentDistance = vertices[currentVertexIndex].currentDistance + edgeWeight;
				vertices[neighbourVertexIndex].predecessor = currentVertexIndex;
				vertexHeap.push(std::make_pair(vertices[neighbourVertexIndex].currentDistance + GetRemainingDistance(neighbourVertexIndex, anEndIndex), neighbourVertexIndex));
			}
		}
	}

	std::vector<int> shortestPath;

	if (foundPath)
	{
		int pathVertexIndex = anEndIndex;

		while (vertices[pathVertexIndex].predecessor >= 0)
		{
			shortestPath.push_back(pathVertexIndex);
			pathVertexIndex = vertices[pathVertexIndex].predecessor;
		}

		shortestPath.push_back(aStartIndex);
	}

	std::reverse(shortestPath.begin(), shortestPath.end());
	return shortestPath;
}