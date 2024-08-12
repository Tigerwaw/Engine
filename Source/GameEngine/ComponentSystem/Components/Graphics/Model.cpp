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

bool Model::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool Model::Deserialize(nl::json& aJsonObject)
{
    if (!aJsonObject.contains("Model")) return false;

    if (aJsonObject.contains("ShouldViewcull"))
    {
        SetViewcull(aJsonObject["ShouldViewcull"].get<bool>());
    }

    SetMesh(AssetManager::Get().GetAsset<MeshAsset>(aJsonObject["Model"].get<std::string>())->mesh);

    if (aJsonObject.contains("Materials"))
    {
        for (int i = 0; i < aJsonObject["Materials"].size(); i++)
        {
            SetMaterialOnSlot(i, AssetManager::Get().GetAsset<MaterialAsset>(aJsonObject["Materials"][i].get<std::string>())->material);
        }
    }

    return true;
}
