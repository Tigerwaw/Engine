#include "GraphicsEngine.pch.h"
#include "GBuffer.h"
#include "Texture.h"

void GBuffer::CreateGBuffer(unsigned aWidth, unsigned aHeight)
{
	myAlbedo = std::make_shared<Texture>();
	GraphicsEngine::Get().CreateTexture("GBuffer_Albedo", aWidth, aHeight, RHITextureFormat::R8G8B8A8_UNORM, *myAlbedo, false, true, true, false, false);
	myMaterial = std::make_shared<Texture>();
	GraphicsEngine::Get().CreateTexture("GBuffer_Material", aWidth, aHeight, RHITextureFormat::R8G8B8A8_UNORM, *myMaterial, false, true, true, false, false);
	myWorldNormal = std::make_shared<Texture>();
	GraphicsEngine::Get().CreateTexture("GBuffer_WorldNormal", aWidth, aHeight, RHITextureFormat::R16G16B16A16_SNORM, *myWorldNormal, false, true, true, false, false);
	myWorldPosition = std::make_shared<Texture>();
	GraphicsEngine::Get().CreateTexture("GBuffer_WorldPosition", aWidth, aHeight, RHITextureFormat::R32G32B32A32_Float, *myWorldPosition, false, true, true, false, false);
}
