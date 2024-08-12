#pragma once
#include "GameEngine/ComponentSystem/Component.h"

#include "GameEngine/EngineDefines.h"
#include "GameEngine/Intersections/AABB3D.hpp"
namespace CU = CommonUtilities;

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
    const CU::AABB3D<float> GetBoundingBox() const;

    void SetViewcull(bool aShouldViewcull) { myShouldViewcull = aShouldViewcull; }
    const bool GetShouldViewcull() const { return myShouldViewcull; }

protected:
    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;
    bool myShouldViewcull = true;
};

