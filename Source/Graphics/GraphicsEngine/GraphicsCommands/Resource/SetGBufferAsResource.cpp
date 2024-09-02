#include "GraphicsEngine.pch.h"
#include "SetGBufferAsResource.h"
#include "Objects/GBuffer.h"
#include "Objects/Texture.h"

SetGBufferAsResource::SetGBufferAsResource()
{
	GBuffer& gBuffer = GraphicsEngine::Get().GetGBuffer();
	myAlbedo = gBuffer.GetAlbedo();
	myMaterial = gBuffer.GetMaterial();
	myWorldNormal = gBuffer.GetWorldNormal();
	myWorldPosition = gBuffer.GetWorldPosition();
}

void SetGBufferAsResource::Execute()
{
	GraphicsEngine::Get().SetTextureResource_PS(0, *myAlbedo);
	GraphicsEngine::Get().SetTextureResource_PS(1, *myMaterial);
	GraphicsEngine::Get().SetTextureResource_PS(2, *myWorldNormal);
	GraphicsEngine::Get().SetTextureResource_PS(3, *myWorldPosition);
}

void SetGBufferAsResource::Destroy()
{
	myAlbedo = nullptr;
	myMaterial = nullptr;
	myWorldNormal = nullptr;
	myWorldPosition = nullptr;
}