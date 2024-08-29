#include "GraphicsEngine.pch.h"
#include "RenderText.h"
#include "AssetManager.h"
#include "Graphics/GraphicsEngine/Objects/Text/Text.h"
#include "Graphics/GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"

RenderText::RenderText(std::shared_ptr<Text> aTextObject)
{
	text = aTextObject;
}

void RenderText::Execute()
{
	GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("PSO_Text")->pso);
	GraphicsEngine::Get().RenderText(*text);
}

void RenderText::Destroy()
{
	text = nullptr;
}
