#include "GraphicsEngine.pch.h"
#include "UpdateLightBuffer.h"

UpdateLightBuffer::UpdateLightBuffer(const LightBuffer& aLightBuffer)
{
	myLightBuffer = aLightBuffer;
}

void UpdateLightBuffer::Execute()
{
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::LightBuffer, myLightBuffer);
}

void UpdateLightBuffer::Destroy()
{
}
