#include "DebugDrawer.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/Objects/DynamicVertexBuffer.h"
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandList.h"
#include "GameEngine/EngineDefines.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

#include <array>

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Warning);
#endif

#define DEBUGDRAWERLOG(Verbosity, Message, ...) LOG(LogDebugDrawer, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogDebugDrawer);

void DebugDrawer::InitializeDebugDrawer()
{
    myLineVertices.emplace_back(DebugLineVertex({ 0, 0, 0 }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));

    myLineBuffer = std::make_shared<DynamicVertexBuffer>();
    if (!myLineBuffer->CreateBuffer("Debug_Line_Buffer", myLineVertices, MAX_DEBUG_LINES))
    {
        DEBUGDRAWERLOG(Error, "Failed to create debug line buffer!");
    }

    myLineVertices.clear();
}

void DebugDrawer::DrawObjects()
{
    if (!myLineVertices.empty())
    {
        GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderDebugLines>(myLineVertices, myLineBuffer);
    }
}

void DebugDrawer::ClearObjects()
{
    if (!myLineVertices.empty())
    {
        myLineVertices.clear();
    }
}

void DebugDrawer::DrawLine(CU::Vector3f aFromPosition, CU::Vector3f aToPosition, CU::Vector4f aColor)
{
    if (myLineVertices.size() >= MAX_DEBUG_LINES)
    {
        if (!myHasWarned)
        {
            myHasWarned = true;
            DEBUGDRAWERLOG(Warning, "Debug Drawer Lines has exceeded its limit and all lines are not being drawn! Either draw less lines or consider increasing the max amount in engine defines.");
        }
        
        return;
    }

    myLineVertices.emplace_back(DebugLineVertex(aFromPosition, aToPosition, aColor));
}

void DebugDrawer::DrawCameraFrustum(std::shared_ptr<Camera> aCamera, CU::Vector4f aColor)
{
    CU::Matrix4x4f camMatrix = aCamera->gameObject->GetComponent<Transform>()->GetMatrix();
    std::array<CU::Vector3f, 8> frustumVolume = aCamera->GetFrustumCorners();

    for (auto& pos : frustumVolume)
    {
        pos = CU::ToVector3(CU::ToVector4(pos, 1.0f) * camMatrix);
    }

    DrawLine(frustumVolume[0], frustumVolume[1], aColor);
    DrawLine(frustumVolume[1], frustumVolume[2], aColor);
    DrawLine(frustumVolume[2], frustumVolume[3], aColor);
    DrawLine(frustumVolume[3], frustumVolume[0], aColor);

    DrawLine(frustumVolume[4], frustumVolume[5], aColor);
    DrawLine(frustumVolume[5], frustumVolume[6], aColor);
    DrawLine(frustumVolume[6], frustumVolume[7], aColor);
    DrawLine(frustumVolume[7], frustumVolume[4], aColor);

    DrawLine(frustumVolume[0], frustumVolume[4], aColor);
    DrawLine(frustumVolume[1], frustumVolume[5], aColor);
    DrawLine(frustumVolume[2], frustumVolume[6], aColor);
    DrawLine(frustumVolume[3], frustumVolume[7], aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<Model> aModel, CU::Vector4f aColor)
{
    CU::Matrix4x4f objectMatrix = aModel->gameObject->GetComponent<Transform>()->GetMatrix();
    CU::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBoxInternal(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<AnimatedModel> aModel, CU::Vector4f aColor)
{
    CU::Matrix4x4f objectMatrix = aModel->gameObject->GetComponent<Transform>()->GetMatrix();
    CU::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBoxInternal(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<DebugModel> aModel, CU::Vector4f aColor)
{
    CU::Matrix4x4f objectMatrix = aModel->gameObject->GetComponent<Transform>()->GetMatrix();
    CU::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBoxInternal(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingBoxInternal(CU::AABB3D<float> aAABB, CU::Matrix4x4f aWorldMatrix, CU::Vector4f aColor)
{
    std::vector<CU::Vector3f> corners = aAABB.GetCorners();

    for (auto& corner : corners)
    {
        corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * aWorldMatrix);
    }

    DrawLine(corners[0], corners[1], aColor);
    DrawLine(corners[1], corners[2], aColor);
    DrawLine(corners[2], corners[3], aColor);
    DrawLine(corners[3], corners[0], aColor);

    DrawLine(corners[4], corners[5], aColor);
    DrawLine(corners[5], corners[6], aColor);
    DrawLine(corners[6], corners[7], aColor);
    DrawLine(corners[7], corners[4], aColor);

    DrawLine(corners[0], corners[4], aColor);
    DrawLine(corners[1], corners[5], aColor);
    DrawLine(corners[2], corners[6], aColor);
    DrawLine(corners[3], corners[7], aColor);
}
