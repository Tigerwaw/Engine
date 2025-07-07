#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"

RenderAnimatedMesh::RenderAnimatedMesh(const AnimMeshRenderData& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(6), "Graphics Command Copy Anim Mesh Data");
    myData = aModelData;
}

RenderAnimatedMesh::RenderAnimatedMesh(AnimMeshRenderData&& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(6), "Graphics Command Move Anim Mesh Data");
    myData = std::move(aModelData);
}

void RenderAnimatedMesh::Execute()
{
    if (!myData.mesh) return;

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.WorldInvT = myData.transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(Math::Matrix4x4<float>) * 128, myData.jointTransforms.data(), sizeof(Math::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    if (myData.psoOverride)
    {
        GraphicsEngine::Get().ChangePipelineState(myData.psoOverride);
        GraphicsEngine::Get().RenderMesh(*myData.mesh, myData.materialList, true);
    }
    else
    {
        GraphicsEngine::Get().RenderMesh(*myData.mesh, myData.materialList);
    }
}

void RenderAnimatedMesh::Destroy()
{
    myData.mesh = nullptr;
    myData.psoOverride = nullptr;
    myData.materialList.~vector();
}