#pragma once
#include <wrl.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "GameEngine/Math/Matrix4x4.hpp"
#include "GameEngine/Math/Vector.hpp"

struct ID3D11Buffer;
struct Vertex;

namespace CU = CommonUtilities;

class Mesh
{
	friend class RenderHardwareInterface;

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
			CU::Matrix4x4<float> BindPoseInverse;
			int Parent;
			std::vector<unsigned> Children;
			std::string Name;
		};

		std::vector<Joint> myJoints;
		std::unordered_map<std::string, size_t> JointNameToIndex;
	};

	Mesh();
	~Mesh();

	void Initialize(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList, std::vector<Element>&& aElementList, Skeleton aSkeleton);

	FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return myVertexBuffer; }
	FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndexBuffer() const { return myIndexBuffer; }
	FORCEINLINE const std::vector<Element>& GetElements() const { return myElements; }
	FORCEINLINE const Skeleton& GetSkeleton() const { return mySkeleton; }

private:
	std::vector<Vertex> myVertices;
	std::vector<unsigned> myIndices;
	std::vector<Element> myElements;
	Skeleton mySkeleton;

	unsigned myPrimitiveTopology = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
};