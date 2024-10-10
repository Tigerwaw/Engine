#include "Enginepch.h"
#include "InstancedModel.h"
#include "Model.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

#undef min
#undef max

InstancedModel::~InstancedModel()
{
}

void InstancedModel::Start()
{
	myMeshTransformBuffer.CreateBuffer("Instanced Model Buffer", myMeshTransforms, 1000);
}

void InstancedModel::Update()
{
}

void InstancedModel::SetMesh(std::shared_ptr<Mesh> aMesh)
{
	myMesh = aMesh;
}

void InstancedModel::AddInstance(CU::Matrix4x4f aTransform)
{
	myMeshTransforms.emplace_back(aTransform);

	myMeshTransformBuffer.UpdateVertexBuffer(myMeshTransforms);

	CU::AABB3D<float> meshAABB = myMesh->GetBoundingBox().GetAABBinNewSpace(gameObject->GetComponent<Transform>()->GetWorldMatrix());
	meshAABB = meshAABB.GetAABBinNewSpace(myMeshTransforms.back());
	CU::Vector3f aabbMin = myBoundingBox.GetMin();
	CU::Vector3f aabbMax = myBoundingBox.GetMax();
	aabbMin.x = std::min(aabbMin.x, meshAABB.GetMin().x);
	aabbMin.y = std::min(aabbMin.y, meshAABB.GetMin().y);
	aabbMin.z = std::min(aabbMin.z, meshAABB.GetMin().z);
	aabbMax.x = std::max(aabbMax.x, meshAABB.GetMax().x);
	aabbMax.y = std::max(aabbMax.y, meshAABB.GetMax().y);
	aabbMax.z = std::max(aabbMax.z, meshAABB.GetMax().z);

	myBoundingBox.InitWithMinAndMax(aabbMin, aabbMax);
}

void InstancedModel::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
	myMaterials.emplace_back(aMaterial);

	if (mySlotToIndex.find(aSlot) != mySlotToIndex.end())
	{
		myMaterials.erase(myMaterials.begin() + mySlotToIndex.at(aSlot));
		mySlotToIndex.erase(aSlot);
	}

	mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

bool InstancedModel::Serialize(nl::json&)
{
	return false;
}

bool InstancedModel::Deserialize(nl::json&)
{
	return false;
}
