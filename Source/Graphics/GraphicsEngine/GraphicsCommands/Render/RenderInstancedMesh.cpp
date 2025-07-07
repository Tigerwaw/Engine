#include "GraphicsEngine.pch.h"
#include "RenderInstancedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderInstancedMesh::RenderInstancedMesh(const InstancedMeshRenderData& aInstancedModelData)
{
    myData = aInstancedModelData;
}

RenderInstancedMesh::RenderInstancedMesh(InstancedMeshRenderData&& aInstancedModelData)
{
    myData = std::move(aInstancedModelData);
}

void RenderInstancedMesh::Execute()
{
    if (!myData.mesh) return;

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.WorldInvT = myData.transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    if (myData.psoOverride)
    {
        GraphicsEngine::Get().ChangePipelineState(myData.psoOverride);
        GraphicsEngine::Get().RenderInstancedMesh(*myData.mesh, myData.meshCount, myData.materialList, *myData.instanceBuffer, true);
    }
    else
    {
        GraphicsEngine::Get().RenderInstancedMesh(*myData.mesh, myData.meshCount, myData.materialList, *myData.instanceBuffer);
    }
}

void RenderInstancedMesh::Destroy()
{
    myData.mesh = nullptr;
    myData.psoOverride = nullptr;
    myData.materialList.~vector();
}
