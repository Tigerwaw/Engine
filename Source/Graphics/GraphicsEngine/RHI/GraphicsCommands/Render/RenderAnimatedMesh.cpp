#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMesh.h"

#include "GraphicsEngine/RHI/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/AnimationBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"

RenderAnimatedMesh::RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->GetParent()->Transform.GetMatrix();
    materialList = aModel->GetMaterials();
    jointTransforms = aModel->GetCurrentPose();
}

void RenderAnimatedMesh::Execute()
{
    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(CU::Matrix4x4<float>) * 128, jointTransforms, sizeof(CU::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    GraphicsEngine::Get().RenderMesh(*mesh, materialList);
}

void RenderAnimatedMesh::Destroy()
{
    mesh = nullptr;
    jointTransforms = nullptr;
    materialList.clear();
}