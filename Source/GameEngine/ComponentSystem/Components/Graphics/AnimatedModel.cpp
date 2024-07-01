#include "GameEngine/ComponentSystem/GameObject.h"

#include "AnimatedModel.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Animation.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Math/Transform.hpp"
#include "GameEngine/Math/Quaternion.hpp"
namespace CU = CommonUtilities;

#include "Logger/Logger.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(ComponentSystem, "ComponentSystem", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(ComponentSystem, "ComponentSystem", Error);
#endif

DEFINE_LOG_CATEGORY(ComponentSystem);

AnimatedModel::~AnimatedModel()
{
    myMesh = nullptr;
    myMaterials.clear();
    mySlotToIndex.clear();
    myAnimationLayers.clear();
}

AnimatedModel::AnimatedModel(std::shared_ptr<Mesh> aMesh)
{
    SetMesh(aMesh);
}

AnimatedModel::AnimatedModel(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial)
{
    SetMesh(aMesh);
    SetMaterialOnSlot(0, aMaterial);
}

void AnimatedModel::Start()
{
}

void AnimatedModel::Update()
{
    if (!myMesh) return;
    if (myAnimationLayers.size() <= 0) return;
    if (!myIsPlaying) return;

    float deltaTime = Engine::GetInstance().GetTimer().GetDeltaTime();
    myAnimTime += deltaTime;
    if (myAnimTime >= myFrametime)
    {
        myAnimTime = 0;

        for (auto& animationLayer : myAnimationLayers)
        {
            UpdateAnimationLayer(animationLayer);
        }
    }
}

void AnimatedModel::SetMesh(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
    AddAnimationLayer(0);
}

void AnimatedModel::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
    myMaterials.emplace_back(aMaterial);
    mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

void AnimatedModel::AddAnimationLayer(unsigned aJointID)
{
    if (!myMesh.get())
    {
        LOG(ComponentSystem, Error, "Could not find mesh!");
        return;
    }

    if (aJointID >= myMesh->GetSkeleton().myJoints.size())
    {
        LOG(ComponentSystem, Error, "Joint index {} out of range!", aJointID);
        return;
    }

    AnimationLayer& newAnimLayer = myAnimationLayers.emplace_back(AnimationLayer());
    myJointNameToLayerIndex.emplace(myMesh->GetSkeleton().myJoints[aJointID].Name, static_cast<unsigned>(myAnimationLayers.size() - 1));
    newAnimLayer.startJointID = aJointID;
}

void AnimatedModel::AddAnimationLayer(std::string aStartJoint)
{
    if (!myMesh.get())
    {
        LOG(ComponentSystem, Error, "Could not find mesh!");
        return;
    }

    if (myMesh->GetSkeleton().JointNameToIndex.find(aStartJoint) == myMesh->GetSkeleton().JointNameToIndex.end())
    {
        LOG(ComponentSystem, Error, "Could not find joint with name {}!", aStartJoint);
        return;
    }

    AnimationLayer& newAnimLayer = myAnimationLayers.emplace_back(AnimationLayer());
    myJointNameToLayerIndex.emplace(aStartJoint, static_cast<unsigned>(myAnimationLayers.size() - 1));
    newAnimLayer.startJointID = static_cast<unsigned>(myMesh->GetSkeleton().JointNameToIndex.at(aStartJoint));
}

void AnimatedModel::SetAnimation(std::shared_ptr<Animation> aNewAnimation, unsigned aStartingFrame, unsigned aLayerIndex, float aBlendTime, bool aShouldLoop)
{
    if (aBlendTime > 0)
    {
        myAnimationLayers[aLayerIndex].isBlending = true;
        myAnimationLayers[aLayerIndex].maxBlendTime = aBlendTime;
        myAnimationLayers[aLayerIndex].currentBlendTime = 0;

        myAnimationLayers[aLayerIndex].nextState.animation = aNewAnimation;
        myAnimationLayers[aLayerIndex].nextState.currentFrame = aStartingFrame;
        myAnimationLayers[aLayerIndex].nextState.currentTime = 0;
        myAnimationLayers[aLayerIndex].nextState.isLooping = aShouldLoop;
        myFrametime = 1.0f / myAnimationLayers[aLayerIndex].nextState.animation->FramesPerSecond;
    }
    else
    {
        myAnimationLayers[aLayerIndex].currentState.animation = aNewAnimation;
        myAnimationLayers[aLayerIndex].currentState.currentFrame = aStartingFrame;
        myAnimationLayers[aLayerIndex].currentState.currentTime = 0;
        myAnimationLayers[aLayerIndex].currentState.isLooping = aShouldLoop;
        myFrametime = 1.0f / myAnimationLayers[aLayerIndex].currentState.animation->FramesPerSecond;
    }
}

void AnimatedModel::SetAnimation(std::shared_ptr<Animation> aNewAnimation, unsigned aStartingFrame, std::string aStartJoint, float aBlendTime, bool aShouldLoop)
{
    if (aStartJoint == "")
    {
        SetAnimation(aNewAnimation, aStartingFrame, 0, aBlendTime, aShouldLoop);
        return;
    }

    if (myJointNameToLayerIndex.find(aStartJoint) == myJointNameToLayerIndex.end())
    {
        LOG(ComponentSystem, Error, "Could not find a layer by the name of {}!", aStartJoint);
        return;
    }

    SetAnimation(aNewAnimation, aStartingFrame, myJointNameToLayerIndex.at(aStartJoint), aBlendTime, aShouldLoop);
}

void AnimatedModel::PlayAnimation()
{
    myIsPlaying = true;
}

void AnimatedModel::StopAnimation()
{
    myIsPlaying = false;
}

void AnimatedModel::UpdateAnimationLayer(AnimationLayer& aAnimationLayer)
{
    UpdateAnimationState(aAnimationLayer.currentState);

    if (aAnimationLayer.isBlending)
    {
        UpdateAnimationState(aAnimationLayer.nextState);

        if (aAnimationLayer.currentBlendTime >= aAnimationLayer.maxBlendTime)
        {
            aAnimationLayer.currentState = aAnimationLayer.nextState;
            aAnimationLayer.isBlending = false;
        }
    }

    CU::Matrix4x4<float> parentBoneMatrix;
    if (aAnimationLayer.startJointID > 0)
    {
        int parentID = myMesh->GetSkeleton().myJoints[aAnimationLayer.startJointID].Parent;
        parentBoneMatrix = myMesh->GetSkeleton().myJoints[parentID].BindPoseInverse.GetFastInverse() * myJointTransforms[parentID];
    }

    UpdateAnimation(aAnimationLayer, aAnimationLayer.startJointID, parentBoneMatrix, myJointTransforms);
}

void AnimatedModel::UpdateAnimationState(AnimationState& aAnimationState)
{
    aAnimationState.currentTime = 0;

    aAnimationState.currentFrame++;
    if (aAnimationState.animation->Frames.size() <= aAnimationState.currentFrame)
    {
        if (aAnimationState.isLooping)
        {
            aAnimationState.currentFrame = 0;
        }
        else
        {
            // Freeze last frame
            aAnimationState.currentFrame = static_cast<unsigned>(aAnimationState.animation->Frames.size() - 1);
        }
    }
}

void AnimatedModel::UpdateAnimation(AnimationLayer& aAnimLayer, unsigned aJointIdx, const CU::Matrix4x4f& aParentJointTransform, std::array<CU::Matrix4x4f, 128>& outTransforms)
{
    Mesh::Skeleton::Joint currentJoint = myMesh->GetSkeleton().myJoints[aJointIdx];

    CU::Matrix4x4f currentFrameJointTransform = aAnimLayer.currentState.animation->Frames[aAnimLayer.currentState.currentFrame].BoneTransforms[currentJoint.Name];
    currentFrameJointTransform = currentFrameJointTransform * aParentJointTransform;
    CU::Matrix4x4f result = currentJoint.BindPoseInverse * currentFrameJointTransform;

    if (aAnimLayer.isBlending)
    {
        CU::Matrix4x4f nextAnimationJointTransform = aAnimLayer.nextState.animation->Frames[aAnimLayer.nextState.currentFrame].BoneTransforms[currentJoint.Name];
        nextAnimationJointTransform = nextAnimationJointTransform * aParentJointTransform;
        CU::Matrix4x4f nextAnimationResult = currentJoint.BindPoseInverse * nextAnimationJointTransform;

        aAnimLayer.currentBlendTime += Engine::GetInstance().GetTimer().GetDeltaTime();
        float blendFactor = aAnimLayer.currentBlendTime / aAnimLayer.maxBlendTime;
        result = BlendJoints(result, nextAnimationResult, blendFactor);
    }

    outTransforms[aJointIdx] = result;

    for (auto& childIndex : currentJoint.Children)
    {
        UpdateAnimation(aAnimLayer, childIndex, currentFrameJointTransform, outTransforms);
    }
}

CU::Matrix4x4f AnimatedModel::BlendJoints(const CU::Matrix4x4f& aCurrentJointTransform, const CU::Matrix4x4f& aNextJointTransform, float aBlendFactor)
{
    const CU::Vector3f T = CU::Vector3f::Lerp(CU::Matrix4x4f::CreateTranslationVector(aCurrentJointTransform), CU::Matrix4x4f::CreateTranslationVector(aNextJointTransform), aBlendFactor);
    const CU::Quatf R = CU::Quatf::Slerp(CU::Quatf(aCurrentJointTransform), CU::Quatf(aNextJointTransform), aBlendFactor);
    const CU::Vector3f S = CU::Vector3f::Lerp(CU::Matrix4x4f::CreateScaleVector(aCurrentJointTransform), CU::Matrix4x4f::CreateScaleVector(aNextJointTransform), aBlendFactor);

    CU::Matrix4x4f currentAnimJointMatrix = CU::Matrix4x4f::CreateScaleMatrix(S) * R.GetRotationMatrix4x4f() * CU::Matrix4x4f::CreateTranslationMatrix(T);
    return currentAnimJointMatrix;
}