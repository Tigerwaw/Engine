#include "GraphicsEngine.pch.h"
#include "RenderDebugMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/DebugModel.h"
#include "AssetManager.h"

#include "Engine.h"
#include "DebugDrawer/DebugDrawer.h"

RenderDebugMesh::RenderDebugMesh(std::shared_ptr<DebugModel> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->gameObject->GetComponent<Transform>()->GetWorldMatrix();
    materialList = aModel->GetMaterials();

    if (GraphicsEngine::Get().DrawBoundingBoxes)
    {
        Engine::Get().GetDebugDrawer().DrawBoundingBox(aModel);
    }
}

void RenderDebugMesh::Execute()
{
    if (!mesh) return;

    ObjectBuffer objBufferData;
    objBufferData.World = transform;
    objBufferData.WorldInvT = transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("PSO_Gizmo")->pso);
    GraphicsEngine::Get().RenderMesh(*mesh, materialList, true);
}

void RenderDebugMesh::Destroy()
{
    mesh = nullptr;
    materialList.~vector();
}