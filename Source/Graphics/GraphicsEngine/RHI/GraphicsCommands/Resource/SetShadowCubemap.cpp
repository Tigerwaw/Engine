#include "GraphicsEngine.pch.h"
#include "SetShadowCubemap.h"
#include "Graphics/GraphicsEngine/RHI/Texture.h"
#include "../ChangePipelineState.h"

SetShadowCubemap::SetShadowCubemap(std::shared_ptr<Texture> aShadowCubemap)
{
	myShadowCubemap = aShadowCubemap;
}

void SetShadowCubemap::Execute()
{
	GraphicsEngine::Get().GetPSO(PipelineStateType::ShadowCube)->DepthStencil = myShadowCubemap;
	GraphicsEngine::Get().ChangePipelineState(PipelineStateType::ShadowCube);
}

void SetShadowCubemap::Destroy()
{
	myShadowCubemap = nullptr;
}
