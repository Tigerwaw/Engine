#include "GraphicsEngine.pch.h"
#include "RenderMesh.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/MaterialBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"

RenderMesh::RenderMesh(std::shared_ptr<Model> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->GetParent()->Transform.GetMatrix();
    materialList = aModel->GetMaterials();
}

void RenderMesh::Execute()
{
    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().RenderMesh(*mesh, materialList);
}

void RenderMesh::Destroy()
{
    mesh = nullptr;
    materialList.clear();
}