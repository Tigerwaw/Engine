#include "GameEngine/ComponentSystem/GameObject.h"

#include "AnimatedModel.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Animation.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

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
    myMesh = aMesh;
}

AnimatedModel::AnimatedModel(std::shared_ptr<Mesh> aMesh, std::shared_ptr<Material> aMaterial)
{
    myMesh = aMesh;
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
            animationLayer.currentTime = 0;

            animationLayer.currentFrame++;
            if (animationLayer.animation->Frames.size() <= animationLayer.currentFrame)
            {
                animationLayer.currentFrame = 0;
            }

            CU::Matrix4x4<float> parentBoneMatrix;
            if (animationLayer.startBoneID > 0)
            {
                int parentID = myMesh->GetSkeleton().myJoints[animationLayer.startBoneID].Parent;
                parentBoneMatrix = myMesh->GetSkeleton().myJoints[parentID].BindPoseInverse.GetFastInverse() * myJointTransforms[parentID];
            }

            UpdateAnimation(animationLayer, animationLayer.startBoneID, parentBoneMatrix, myJointTransforms);
        }
    }
}

void AnimatedModel::SetMesh(std::shared_ptr<Mesh> aMesh)
{
    myMesh = aMesh;
}

void AnimatedModel::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
    myMaterials.emplace_back(aMaterial);
    mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

void AnimatedModel::SetBaseAnimation(std::shared_ptr<Animation> aNewAnimation, unsigned aStartingFrame)
{
    myAnimationLayers[0].animation = aNewAnimation;
    myAnimationLayers[0].currentFrame = aStartingFrame;
    myAnimationLayers[0].currentTime = 0;
    myFrametime = 1.0f / myAnimationLayers[0].animation->FramesPerSecond;
}

void AnimatedModel::AddAnimation(std::shared_ptr<Animation> aNewAnimation, std::string aStartBone, unsigned aStartingFrame)
{
    if (!myMesh.get())
    {
        LOG(ComponentSystem, Error, "Could not add animation to mesh!");
        return;
    }

    AnimationLayer& newAnimLayer = myAnimationLayers.emplace_back(AnimationLayer());
    newAnimLayer.animation = aNewAnimation;
    newAnimLayer.currentFrame = aStartingFrame;
    newAnimLayer.startBoneID = 0;
    myFrametime = 1.0f / myAnimationLayers[0].animation->FramesPerSecond;

    if (myMesh->GetSkeleton().JointNameToIndex.find(aStartBone) != myMesh->GetSkeleton().JointNameToIndex.end())
    {
        newAnimLayer.startBoneID = static_cast<unsigned>(myMesh->GetSkeleton().JointNameToIndex.at(aStartBone));
    }
}

void AnimatedModel::PlayAnimation()
{
    myIsPlaying = true;
}

void AnimatedModel::StopAnimation()
{
    myIsPlaying = false;
}

void AnimatedModel::UpdateAnimation(AnimationLayer aAnimLayer, unsigned aJointIdx, const CU::Matrix4x4<float>& aParentJointTransform, std::array<CU::Matrix4x4f, 128>& outTransforms)
{
    Mesh::Skeleton::Joint currentJoint = myMesh->GetSkeleton().myJoints[aJointIdx];

    CU::Matrix4x4<float> currentFrameJointTransform = aAnimLayer.animation->Frames[aAnimLayer.currentFrame].BoneTransforms[currentJoint.Name];
    currentFrameJointTransform = currentFrameJointTransform * aParentJointTransform;
    CU::Matrix4x4<float> result = currentJoint.BindPoseInverse * currentFrameJointTransform;
    outTransforms[aJointIdx] = result;

    for (auto& childIndex : currentJoint.Children)
    {
        UpdateAnimation(aAnimLayer, childIndex, currentFrameJointTransform, outTransforms);
    }
}