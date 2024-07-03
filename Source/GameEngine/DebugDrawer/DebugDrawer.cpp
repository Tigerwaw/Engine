#include "DebugDrawer.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandList.h"
#include "GameEngine/EngineDefines.h"

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

    Line line;
    line.color = aColor;
    line.fromPosition = CU::ToVector4(aFromPosition, 1.0f);
    line.toPosition = CU::ToVector4(aToPosition, 1.0f);
    myLines.emplace_back(line);
}
