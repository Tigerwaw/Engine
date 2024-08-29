#include "GraphicsEngine.pch.h"
#include "RenderDebugMesh.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/MaterialBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "AssetManager/AssetManager.h"

#include "GameEngine/Engine.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"

RenderDebugMesh::RenderDebugMesh(std::shared_ptr<DebugModel> aModel)
{
    if (!aModel.get()) return;

    mesh = aModel->GetMesh();
    transform = aModel->gameObject->GetComponent<Transform>()->GetMatrix();
    materialList = aModel->GetMaterials();

    if (GraphicsEngine::Get().DrawBoundingBoxes)
    {
        Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(aModel);
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