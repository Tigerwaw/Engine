#include "DebugDrawer.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandList.h"
#include "GameEngine/EngineDefines.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "GameEngine/Math/Transform.hpp"
#include "GameEngine/ComponentSystem/GameObject.h"

#include <array>

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Warning);
#endif

#define DEBUGDRAWERLOG(Verbosity, Message, ...) LOG(LogDebugDrawer, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogDebugDrawer);

void DebugDrawer::DrawObjects()
{
    if (!myLines.empty())
    {
        GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateDebugBuffer>(myLines);
    }
}

void DebugDrawer::ClearObjects()
{
    if (!myLines.empty())
    {
        myLines.clear();
    }
}

void DebugDrawer::DrawLine(CU::Vector3f aFromPosition, CU::Vector3f aToPosition, CU::Vector4f aColor)
{
    if (myLines.size() >= MAX_DEBUG_LINES)
    {
        DEBUGDRAWERLOG(Warning, "Maximum amount of debug lines has been reached and current line will not be drawn! Consider raising the max amount in EngineDefines if needed.");
        return;
    }

    DebugLine line;
    line.color = aColor;
    line.fromPosition = CU::ToVector4(aFromPosition, 1.0f);
    line.toPosition = CU::ToVector4(aToPosition, 1.0f);
    myLines.emplace_back(line);
}

void DebugDrawer::DrawCameraFrustum(std::shared_ptr<Camera> aCamera, CU::Vector4f aColor)
{
    CU::Matrix4x4f camMatrix = aCamera->GetParent()->Transform.GetMatrix();
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
    CU::Matrix4x4f objectMatrix = aModel->GetParent()->Transform.GetMatrix();
    CU::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBoxInternal(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<AnimatedModel> aModel, CU::Vector4f aColor)
{
    CU::Matrix4x4f objectMatrix = aModel->GetParent()->Transform.GetMatrix();
    CU::AABB3D<float> boundingBox = aModel->GetBoundingBox();
    DrawBoundingBoxInternal(boundingBox, objectMatrix, aColor);
}

void DebugDrawer::DrawBoundingBox(std::shared_ptr<DebugModel> aModel, CU::Vector4f aColor)
{
    CU::Matrix4x4f objectMatrix = aModel->GetParent()->Transform.GetMatrix();
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
