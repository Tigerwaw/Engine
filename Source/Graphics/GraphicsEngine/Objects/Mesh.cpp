#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "Mesh.h"
#include "Vertices/Vertex.h"

using namespace Microsoft::WRL;

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::Initialize(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList, std::vector<Element>&& aElementList, Skeleton aSkeleton)
{
	myVertices = std::move(aVertexList);
	myIndices = std::move(aIndexList);
	myElements = std::move(aElementList);
	mySkeleton = aSkeleton;
	GraphicsEngine::Get().GetResourceVendor().CreateVertexBuffer("Vertex Buffer", myVertices, myVertexBuffer);
	GraphicsEngine::Get().GetResourceVendor().CreateIndexBuffer("Index Buffer", myIndices, myIndexBuffer);
}

void Mesh::InitBoundingBox(Math::Vector3f aMinPoint, Math::Vector3f aMaxPoint)
{
	myBoundingBox.InitWithMinAndMax(aMinPoint, aMaxPoint);
}
