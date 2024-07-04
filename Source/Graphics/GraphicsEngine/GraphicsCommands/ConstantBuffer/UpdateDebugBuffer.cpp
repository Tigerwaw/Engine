#include "GraphicsEngine.pch.h"
#include "UpdateDebugBuffer.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/DebugBuffer.h"

UpdateDebugBuffer::UpdateDebugBuffer(std::vector<DebugLine> aLineList)
{
	myLineList = aLineList;
}

void UpdateDebugBuffer::Execute()
{
	DebugBuffer debugBuffer;

	for (int i = 0; i < myLineList.size(); i++)
	{
		debugBuffer.Lines[i].Color = myLineList[i].color;
		debugBuffer.Lines[i].From = myLineList[i].fromPosition;
		debugBuffer.Lines[i].To = myLineList[i].toPosition;
	}
	
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::DebugBuffer, debugBuffer);
	GraphicsEngine::Get().RenderDebugLine(static_cast<unsigned>(myLineList.size()));
}

void UpdateDebugBuffer::Destroy()
{
	myLineList.clear();
}
