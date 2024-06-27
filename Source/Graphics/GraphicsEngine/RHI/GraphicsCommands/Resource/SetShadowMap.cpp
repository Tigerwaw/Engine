#include "GraphicsEngine.pch.h"
#include "SetShadowMap.h"
#include "Graphics/GraphicsEngine/RHI/Texture.h"
#include "../ChangePipelineState.h"

SetShadowMap::SetShadowMap(std::shared_ptr<Texture> aShadowMap)
{
	myShadowMap = aShadowMap;
}

void SetShadowMap::Execute()
{
	GraphicsEngine::Get().GetPSO(PipelineStateType::Shadow)->DepthStencil = myShadowMap;
	GraphicsEngine::Get().ChangePipelineState(PipelineStateType::Shadow);
}

void SetShadowMap::Destroy()
{
	myShadowMap = nullptr;
}
