#pragma once
#include "Math/Matrix4x4.hpp"
#include "Math/Vector.hpp"
#include "Math/AABB3D.hpp"

struct ID3D11Buffer;
struct Vertex;

class Mesh
{
public:
	struct Element
	{
		unsigned VertexOffset = 0;
		unsigned IndexOffset = 0;
		unsigned NumVertices = 0;
		unsigned NumIndices = 0;
		unsigned MaterialIndex = 0;
	};

	struct Skeleton
	{
		struct Joint
		{
			Math::Matrix4x4<float> BindPoseInverse;
			int Parent = 0;
			std::vector<unsigned> Children;
			std::string Name;
		};

		std::vector<Joint> myJoints;
		std::unordered_map<std::string, size_t> JointNameToIndex;
	};

	Mesh();
	~Mesh();

	void Initialize(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList, std::vector<Element>&& aElementList, Skeleton aSkeleton);
	void InitBoundingBox(Math::Vector3f aMinPoint, Math::Vector3f aMaxPoint);

	FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return myVertexBuffer; }
	FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndexBuffer() const { return myIndexBuffer; }
	FORCEINLINE const std::vector<Element>& GetElements() const { return myElements; }
	FORCEINLINE const Skeleton& GetSkeleton() const { return mySkeleton; }
	FORCEINLINE const Math::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

private:
	std::vector<Vertex> myVertices;
	std::vector<unsigned> myIndices;
	std::vector<Element> myElements;
	Skeleton mySkeleton;
	Math::AABB3D<float> myBoundingBox;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
};