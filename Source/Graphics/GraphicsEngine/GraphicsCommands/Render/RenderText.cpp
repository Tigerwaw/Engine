#include "GraphicsEngine.pch.h"
#include "RenderText.h"
#include "Objects/Text/Text.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderText::RenderText(const TextData& aTextData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderText Copy Constructor");
	myData = aTextData;
}

RenderText::RenderText(TextData&& aTextData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderText Move Constructor");
	myData = std::move(aTextData);
}

void RenderText::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderText Execute");
	GraphicsEngine::Get().GetDrawer().RenderText(*myData.text);
}

void RenderText::Destroy()
{
	myData.text = nullptr;
}
