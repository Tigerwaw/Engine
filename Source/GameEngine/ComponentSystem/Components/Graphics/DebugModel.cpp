#include "DebugModel.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GameEngine/ComponentSystem/GameObject.h"

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
