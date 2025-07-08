#include "GraphicsEngine.pch.h"
#include "UpdatePostProcessBuffer.h"

UpdatePostProcessBuffer::UpdatePostProcessBuffer()
{
}

void UpdatePostProcessBuffer::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdatePostProcessBuffer Execute");
	GraphicsEngine& gfx = GraphicsEngine::Get();

	PostProcessBuffer ppBufferData;
	const std::vector<Math::Vector4f>& randomKernel = gfx.GetRandomKernel();
	unsigned kernelSize = static_cast<unsigned>(randomKernel.size());
	unsigned dataSize = sizeof(Math::Vector4f) * kernelSize;
	
	memcpy_s(ppBufferData.RandomKernel, dataSize, randomKernel.data(), dataSize);

	ppBufferData.KernelSize = kernelSize;
	ppBufferData.SSAOEnabled = static_cast<unsigned>(gfx.SSAOEnabled);
	ppBufferData.BloomFunction = static_cast<unsigned>(gfx.BloomFunction);
	ppBufferData.LuminanceFunction = static_cast<unsigned>(gfx.LuminanceFunction);
	ppBufferData.SSAONoisePower = gfx.SSAONoisePower;
	ppBufferData.SSAORadius = gfx.SSAORadius;
	ppBufferData.SSAOBias = gfx.SSAOBias;
	ppBufferData.BloomStrength = gfx.BloomStrength;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::PostProcessBuffer, ppBufferData);
}

void UpdatePostProcessBuffer::Destroy()
{
}