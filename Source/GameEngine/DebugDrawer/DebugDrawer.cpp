#include "DebugDrawer.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandList.h"
#include "GameEngine/EngineDefines.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
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

void DebugDrawer::DrawCameraFrustum(std::shared_ptr<Camera> aCamera)
{
    CU::Matrix4x4f camMatrix = aCamera->GetParent()->Transform.GetMatrix();
    std::array<CU::Vector3f, 8> frustumVolume = aCamera->GetFrustumVolume();

    for (auto& pos : frustumVolume)
    {
        pos = CU::ToVector3(CU::ToVector4(pos, 1.0f) * camMatrix);
    }

    DrawLine(frustumVolume[0], frustumVolume[1]);
    DrawLine(frustumVolume[1], frustumVolume[2]);
    DrawLine(frustumVolume[2], frustumVolume[3]);
    DrawLine(frustumVolume[3], frustumVolume[0]);

    DrawLine(frustumVolume[4], frustumVolume[5]);
    DrawLine(frustumVolume[5], frustumVolume[6]);
    DrawLine(frustumVolume[6], frustumVolume[7]);
    DrawLine(frustumVolume[7], frustumVolume[4]);

    DrawLine(frustumVolume[0], frustumVolume[4]);
    DrawLine(frustumVolume[1], frustumVolume[5]);
    DrawLine(frustumVolume[2], frustumVolume[6]);
    DrawLine(frustumVolume[3], frustumVolume[7]);
}
