#include "GraphicsEngine.pch.h"
#include "RenderMesh.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/MaterialBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"

#include "GameEngine/Engine.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"

RenderMesh::RenderMesh(std::shared_ptr<Model> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    materialList = aModel->GetMaterials();
    customShaderParams_1 = aModel->GetCustomShaderData_1();
    customShaderParams_2 = aModel->GetCustomShaderData_2();

    if (GraphicsEngine::Get().DrawBoundingBoxes)
    {
        Engine::Get().GetDebugDrawer().DrawBoundingBox(aModel);
    }
}

RenderMesh::RenderMesh(std::shared_ptr<Model> aModel, std::shared_ptr<PipelineStateObject> aPSOoverride) : RenderMesh(aModel)
{
    psoOverride = aPSOoverride;
}

void RenderMesh::Execute()
{
    if (!mesh) return;

    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    objBufferData.customData_1 = customShaderParams_1;
    objBufferData.customData_2 = customShaderParams_2;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

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

void RenderMesh::Destroy()
{
    mesh = nullptr;
    psoOverride = nullptr;
    materialList.~vector();
}