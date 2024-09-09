#include "GraphicsEngine.pch.h"
#include "RenderDebugLines.h"
#include "GraphicsEngine/Objects/DynamicVertexBuffer.h"
#include "GraphicsEngine/Objects/Vertices/DebugLineVertex.h"
#include "AssetManager/AssetManager.h"

RenderDebugLines::RenderDebugLines(std::vector<DebugLineVertex> aLineVertices, std::shared_ptr<DynamicVertexBuffer> aLineBuffer)
{
	myLineVertices = aLineVertices;
	myLineBuffer = aLineBuffer;
}

void RenderDebugLines::Execute()
{
	GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("PSO_DebugLine")->pso);
	GraphicsEngine::Get().UpdateDynamicVertexBuffer(myLineVertices, *myLineBuffer);
	GraphicsEngine::Get().RenderDebugLines(*myLineBuffer, static_cast<unsigned>(myLineVertices.size()));
}

void RenderDebugLines::Destroy()
{
	myLineVertices.~vector();
	myLineBuffer = nullptr;
}