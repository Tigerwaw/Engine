#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMesh.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/AnimationBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"

#include "GameEngine/Engine.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"

RenderAnimatedMesh::RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    materialList = aModel->GetMaterials();
    jointTransforms = aModel->GetCurrentPose();
    
    if (GraphicsEngine::Get().DrawBoundingBoxes)
    {
        Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(aModel);
    }
}

RenderAnimatedMesh::RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel, std::shared_ptr<PipelineStateObject> aPSOoverride) : RenderAnimatedMesh(aModel)
{
    psoOverride = aPSOoverride;
}

void RenderAnimatedMesh::Execute()
{
    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(CU::Matrix4x4<float>) * 128, jointTransforms.data(), sizeof(CU::Matrix4x4<float>) * 128);
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
    materialList.clear();
}