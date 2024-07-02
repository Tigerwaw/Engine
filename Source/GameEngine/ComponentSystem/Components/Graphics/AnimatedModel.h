#pragma once
#include "GameEngine/ComponentSystem/Component.h"

#include <memory>
#include <vector>
#include <array>
#include <unordered_map>

// The reason why this class doesn't inherit from the Model class is that it creates problems with type-casting.
// (Calling GetComponent<Model> would return a pointer to an instance of AnimatedModel, which may not be desired)

class Mesh;
class Material;
struct Animation;

class AnimatedModel : public Component
{
public:
    struct AnimationState
    {
        std::shared_ptr<Animation> animation;
        unsigned currentFrame = 0;
        float frameTime = 0;
        float currentTime = 0;
        bool isLooping = false;
    };

    struct AnimationLayer
    {
        AnimationState currentState;
        AnimationState nextState;
        std::array<CU::Matrix4x4f, 128> currentPose;
        unsigned startJointID = 0;
        bool isBlending = false;
        float currentBlendTime = 0;
        float maxBlendTime = 0;
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

    void AddAnimationLayer(unsigned aJointID = 0);
    void AddAnimationLayer(std::string aStartJoint = "");
    void SetAnimation(std::shared_ptr<Animation> aNewAnimation, unsigned aStartingFrame = 0, unsigned aLayerIndex = 0, float aBlendTime = 0, bool aShouldLoop = false);
    void SetAnimation(std::shared_ptr<Animation> aNewAnimation, unsigned aStartingFrame = 0, std::string aStartJoint = "", float aBlendTime = 0, bool aShouldLoop = false);
    void PlayAnimation();
    void StopAnimation();

    std::array<CU::Matrix4x4f, 128> GetCurrentPose() { return myJointTransforms; }
private:
    void UpdateAnimationLayer(AnimationLayer& aAnimationLayer);
    void UpdateAnimationState(AnimationState& aAnimationState);
    void UpdateAnimation(AnimationLayer& aAnimLayer, unsigned aJointIdx, const CU::Matrix4x4f& aParentJointTransform, std::array<CU::Matrix4x4f, 128>& outTransforms);
    void UpdatePose(AnimationLayer& aAnimLayer);
    void BlendPoses(AnimationLayer& aAnimLayer, float aBlendFactor);

    std::shared_ptr<Mesh> myMesh = nullptr;
    std::vector<std::shared_ptr<Material>> myMaterials;
    std::unordered_map<unsigned, unsigned> mySlotToIndex;

    std::vector<AnimationLayer> myAnimationLayers;
    std::unordered_map<std::string, unsigned> myJointNameToLayerIndex;

    std::array<CU::Matrix4x4f, 128> myJointTransforms;
    bool myIsPlaying = false;
};

