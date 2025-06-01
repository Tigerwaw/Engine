#include "GraphicsEngine.pch.h"
#include "RenderDebugLines.h"
#include "Objects/DynamicVertexBuffer.h"
#include "Objects/Vertices/DebugLineVertex.h"

RenderDebugLines::RenderDebugLines(std::vector<DebugLineVertex> aLineVertices, std::shared_ptr<DynamicVertexBuffer> aLineBuffer)
{
	myLineVertices = aLineVertices;
	myLineBuffer = aLineBuffer;
}

void RenderDebugLines::Execute()
{
	GraphicsEngine::Get().UpdateDynamicVertexBuffer(myLineVertices, *myLineBuffer);
	GraphicsEngine::Get().RenderDebugLines(*myLineBuffer, static_cast<unsigned>(myLineVertices.size()));
}

void RenderDebugLines::Destroy()
{
	myLineVertices.~vector();
	myLineBuffer = nullptr;
}