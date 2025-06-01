#include "GraphicsEngine.pch.h"
#include "UpdateFrameBuffer.h"

UpdateFrameBuffer::UpdateFrameBuffer(const FrameBuffer& aFrameBuffer)
{
	myFrameBuffer = aFrameBuffer;
}

void UpdateFrameBuffer::Execute()
{
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, myFrameBuffer);
}

void UpdateFrameBuffer::Destroy()
{
}