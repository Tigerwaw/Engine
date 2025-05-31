#include "GraphicsEngine.pch.h"
#include "RenderInstancedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/InstancedModel.h"

#include "Engine.h"
#include "DebugDrawer/DebugDrawer.h"

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
