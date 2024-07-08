#include "GraphicsEngine.pch.h"
#include "Graphics\GraphicsEngine\GraphicsEngine.h"
#include "Mesh.h"
#include "Vertex.h"

using namespace Microsoft::WRL;

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::Initialize(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList, std::vector<Element>&& aElementList, Skeleton aSkeleton)
{
	myVertices = std::move(aVertexList);
	myIndices = std::move(aIndexList);
	myElements = std::move(aElementList);
	mySkeleton = aSkeleton;
	GraphicsEngine::Get().CreateVertexBuffer("Vertex Buffer", myVertices, myVertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer("Index Buffer", myIndices, myIndexBuffer);
}

void Mesh::InitBoundingBox(CU::Vector3f aMinPoint, CU::Vector3f aMaxPoint)
{
	myBoundingBox.InitWithMinAndMax(aMinPoint, aMaxPoint);
}
