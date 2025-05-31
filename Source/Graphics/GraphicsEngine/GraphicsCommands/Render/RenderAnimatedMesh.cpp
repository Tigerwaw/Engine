#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"

#include "Engine.h"
#include "DebugDrawer/DebugDrawer.h"

RenderAnimatedMesh::RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    materialList = aModel->GetMaterials();
    jointTransforms = aModel->GetCurrentPose();
    
    if (GraphicsEngine::Get().DrawBoundingBoxes)
    {
        Engine::Get().GetDebugDrawer().DrawBoundingBox(aModel);
    }
}

RenderAnimatedMesh::RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel, std::shared_ptr<PipelineStateObject> aPSOoverride) : RenderAnimatedMesh(aModel)
{
    psoOverride = aPSOoverride;
}

void RenderAnimatedMesh::Execute()
{
    if (!mesh) return;

    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(Math::Matrix4x4<float>) * 128, jointTransforms.data(), sizeof(Math::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    if (psoOverride)
    {
        GraphicsEngine::Get().ChangePipelineState(psoOverride);
        GraphicsEngine::Get().RenderMesh(*mesh, materialList, true);
    }
    else
    {
        GraphicsEngine::Get().RenderMesh(*mesh, materialList);
    }
}

void RenderAnimatedMesh::Destroy()
{
    mesh = nullptr;
    materialList.~vector();
}