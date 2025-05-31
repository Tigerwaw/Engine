#pragma once
#include "ComponentSystem/Component.h"

#include "EngineDefines.h"
#include "Math/AABB3D.hpp"


class Mesh;
class Material;

class DebugModel : public Component
{
public:
    ~DebugModel() override;
    DebugModel() = default;
    DebugModel(std::shared_ptr<Mesh> aMesh);
    DebugModel(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial);

    void Start() override;
    void Update() override;

    void SetMesh(std::shared_ptr<Mesh> aMesh);
    std::shared_ptr<Mesh> GetMesh() { return myMesh; }

    void SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial);
    std::shared_ptr<Material> GetMaterialOnSlot(unsigned aSlot) { return myMaterials[mySlotToIndex[aSlot]]; }
    std::vector<std::shared_ptr<Material>> GetMaterials() { return myMaterials; }
    const Math::AABB3D<float> GetBoundingBox() const;

protected:
    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;
};