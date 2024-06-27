#pragma once
#include "GameEngine/ComponentSystem/Component.h"

#include <memory>
#include <vector>
#include <unordered_map>

// The reason why this class doesn't inherit from the Model class is that it creates problems with type-casting.
// (Calling GetComponent<Model> would return a pointer to an instance of AnimatedModel, which may not be desired)

class Mesh;
class Material;
struct Animation;

class AnimatedModel : public Component
{
public:
    struct AnimationLayer
    {
        std::shared_ptr<Animation> animation;
        unsigned currentFrame = 0;
        unsigned startBoneID = 0;
        float frameTime = 0;
        float currentTime = 0;
    };

    ~AnimatedModel() override;
    AnimatedModel() = default;
    AnimatedModel(std::shared_ptr<Mesh> aMesh);
    AnimatedModel(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial);

    void Start() override;
    void Update() override;

    void SetMesh(std::shared_ptr<Mesh> aMesh);
    std::shared_ptr<Mesh> GetMesh() { return myMesh; }

    void SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial);
    std::shared_ptr<Material> GetMaterialOnSlot(unsigned aSlot) { return myMaterials[mySlotToIndex[aSlot]]; }
    std::vector<std::shared_ptr<Material>> GetMaterials() { return myMaterials; }

    void SetBaseAnimation(std::shared_ptr<Animation> aNewAnimation, unsigned aStartingFrame = 0);
    void AddAnimation(std::shared_ptr<Animation> aNewAnimation, std::string aStartBone = "", unsigned aStartingFrame = 0);
    void PlayAnimation();
    void StopAnimation();

    CU::Matrix4x4<float>* GetCurrentPose() { return myJointTransforms; }
private:
    void UpdateAnimation(AnimationLayer aAnimLayer, unsigned aJointIdx, const CU::Matrix4x4<float>& aParentJointTransform, CU::Matrix4x4<float>* outTransforms);

    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;

    std::vector<AnimationLayer> myAnimationLayers;

    CU::Matrix4x4<float> myJointTransforms[128];
    bool myIsPlaying = false;
    float myAnimTime = 0;
    float myFrametime = 0;
};

