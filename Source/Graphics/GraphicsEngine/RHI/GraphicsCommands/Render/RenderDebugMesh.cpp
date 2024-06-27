#include "GraphicsEngine.pch.h"
#include "RenderDebugMesh.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/RHI/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/MaterialBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"

RenderDebugMesh::RenderDebugMesh(std::shared_ptr<DebugModel> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->GetParent()->Transform.GetMatrix();
    materialList = aModel->GetMaterials();
}

void RenderDebugMesh::Execute()
{
    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().RenderMesh(*mesh, materialList);
}

void RenderDebugMesh::Destroy()
{
    mesh = nullptr;
    materialList.clear();
}