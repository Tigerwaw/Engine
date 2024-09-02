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
    void SetCastShadows(bool aCastShadows) { myCastShadows = aCastShadows; }
    const bool GetCastShadows() const { return myCastShadows; }

    void SetCustomShaderData_1(CU::Vector3f aCustomShaderData) { myCustomShaderData_1 = aCustomShaderData; }
    void SetCustomShaderData_2(CU::Vector4f aCustomShaderData) { myCustomShaderData_2 = aCustomShaderData; }
    const CU::Vector3f& GetCustomShaderData_1() const { return myCustomShaderData_1; }
    const CU::Vector4f& GetCustomShaderData_2() const { return myCustomShaderData_2; }

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;

protected:
    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;
    bool myShouldViewcull = true;
    bool myCastShadows = true;

    CU::Vector3f myCustomShaderData_1;
    CU::Vector4f myCustomShaderData_2;
};

