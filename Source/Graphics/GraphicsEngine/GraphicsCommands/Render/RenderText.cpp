#include "GraphicsEngine.pch.h"
#include "RenderText.h"
#include "Objects/Text/Text.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderText::RenderText(const TextData& aTextData)
{
	myData = aTextData;
}

void RenderText::Execute()
{
	GraphicsEngine::Get().RenderText(*myData.text);
}

void RenderText::Destroy()
{
	myData.text = nullptr;
}
