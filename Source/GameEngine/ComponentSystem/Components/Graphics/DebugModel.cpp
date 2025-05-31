#include "Enginepch.h"

#include "DebugModel.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "ComponentSystem/GameObject.h"

DebugModel::~DebugModel()
{
    myMesh = nullptr;
    myMaterials.clear();
    mySlotToIndex.clear();
}

DebugModel::DebugModel(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
}

DebugModel::DebugModel(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial)
{
    myMesh = aMesh;
    SetMaterialOnSlot(0, aMaterial);
}

void DebugModel::Start()
{
}

void DebugModel::Update()
{
}

void DebugModel::SetMesh(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
}

void DebugModel::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
    myMaterials.emplace_back(aMaterial);
    mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

const Math::AABB3D<float> DebugModel::GetBoundingBox() const
{
    if (!myMesh)
    {
        return Math::AABB3D<float>();
    }

    return myMesh->GetBoundingBox();
}
