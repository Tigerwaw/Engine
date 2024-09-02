#include "GraphicsEngine.pch.h"
#include "SetGBufferAsRenderTarget.h"
#include "Objects/GBuffer.h"
#include "Objects/Texture.h"

SetGBufferAsRenderTarget::SetGBufferAsRenderTarget()
{
	GBuffer& gBuffer = GraphicsEngine::Get().GetGBuffer();
	myAlbedo = gBuffer.GetAlbedo();
	myMaterial = gBuffer.GetMaterial();
	myWorldNormal = gBuffer.GetWorldNormal();
	myWorldPosition = gBuffer.GetWorldPosition();
}

void SetGBufferAsRenderTarget::Execute()
{
	std::vector<std::shared_ptr<Texture>> RTs;
	RTs.emplace_back(myAlbedo);
	RTs.emplace_back(myMaterial);
	RTs.emplace_back(myWorldNormal);
	RTs.emplace_back(myWorldPosition);
	GraphicsEngine::Get().SetRenderTargets(RTs, GraphicsEngine::Get().GetDepthBuffer(), true, true);
}

void SetGBufferAsRenderTarget::Destroy()
{
	myAlbedo = nullptr;
	myMaterial = nullptr;
	myWorldNormal = nullptr;
	myWorldPosition = nullptr;
}