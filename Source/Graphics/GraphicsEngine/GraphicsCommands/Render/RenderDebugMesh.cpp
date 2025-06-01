#include "GraphicsEngine.pch.h"
#include "RenderDebugMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderDebugMesh::RenderDebugMesh(const DebugMeshRenderData& aModelData)
{
    myData = aModelData;

   /* mesh = aModel->GetMesh();
    transform = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    materialList = aModel->GetMaterials();

    if (GraphicsEngine::Get().DrawBoundingBoxes)
    {
        Engine::Get().GetDebugDrawer().DrawBoundingBox(aModel);
    }*/
}

void RenderDebugMesh::Execute()
{
    if (!myData.mesh) return;

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.WorldInvT = myData.transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().RenderMesh(*myData.mesh, myData.materialList, true);
}

void RenderDebugMesh::Destroy()
{
    myData.mesh = nullptr;
    myData.materialList.~vector();
}