#pragma once
#include "GameEngine/ComponentSystem/Component.h"

#include <memory>
#include <vector>
#include <unordered_map>

#include "GameEngine/EngineDefines.h"

class Mesh;
class Material;

class Model : public Component
{
public:
    ~Model() override;
    Model() = default;
    Model(std::shared_ptr<Mesh> aMesh);
    Model(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial);

    void Start() override;
    void Update() override;

    void SetMesh(std::shared_ptr<Mesh> aMesh);
    std::shared_ptr<Mesh> GetMesh() { return myMesh; }

    void SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial);
    std::shared_ptr<Material>& GetMaterialOnSlot(unsigned aSlot) { return myMaterials[mySlotToIndex[aSlot]]; }
    std::vector<std::shared_ptr<Material>>& GetMaterials() { return myMaterials; }

protected:
    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;
};

