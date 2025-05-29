#include "GraphicsEngine.pch.h"
#include "RenderInstancedMesh.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/MaterialBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/InstancedModel.h"

#include "GameEngine/Engine.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"

RenderInstancedMesh::RenderInstancedMesh(std::shared_ptr<InstancedModel> aInstancedModel)
{
    if (!aInstancedModel.get()) return;

    mesh = aInstancedModel->GetMesh();
    transform = aInstancedModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    materialList = aInstancedModel->GetMaterials();
    instanceBuffer = &aInstancedModel->GetInstanceBuffer();
    meshCount = aInstancedModel->GetMeshCount();

    if (GraphicsEngine::Get().DrawBoundingBoxes)
    {
        Engine::Get().GetDebugDrawer().DrawBoundingBox(aInstancedModel->GetBoundingBox());
    }
}

RenderInstancedMesh::RenderInstancedMesh(std::shared_ptr<InstancedModel> aInstancedModel, std::shared_ptr<PipelineStateObject> aPSOoverride) : RenderInstancedMesh(aInstancedModel)
{
    psoOverride = aPSOoverride;
}

void RenderInstancedMesh::Execute()
{
    if (!mesh) return;

    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    if (psoOverride)
    {
        GraphicsEngine::Get().ChangePipelineState(psoOverride);
        GraphicsEngine::Get().RenderInstancedMesh(*mesh, meshCount, materialList, *instanceBuffer, true);
    }
    else
    {
        GraphicsEngine::Get().RenderInstancedMesh(*mesh, meshCount, materialList, *instanceBuffer);
    }
}

void RenderInstancedMesh::Destroy()
{
    mesh = nullptr;
    psoOverride = nullptr;
    materialList.~vector();
}
