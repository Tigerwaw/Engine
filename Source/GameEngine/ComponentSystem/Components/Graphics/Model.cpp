#include "Enginepch.h"

#include "Model.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GameEngine/ComponentSystem/GameObject.h"

Model::~Model()
{
    myMesh = nullptr;
    myMaterials.clear();
    mySlotToIndex.clear();
}

Model::Model(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
}

Model::Model(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial)
{
    myMesh = aMesh;
    SetMaterialOnSlot(0, aMaterial);
}

void Model::Start()
{
}

void Model::Update()
{
}

void Model::SetMesh(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
}

void Model::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
    myMaterials.emplace_back(aMaterial);
    mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

const CU::AABB3D<float> Model::GetBoundingBox() const
{
    if (!myMesh)
    {
        return CU::AABB3D<float>();
    }

    return myMesh->GetBoundingBox();
}
