#include "GraphicsEngine.pch.h"
#include "RenderFullscreenQuad.h"

RenderFullscreenQuad::RenderFullscreenQuad()
{
}

void RenderFullscreenQuad::Execute()
{
	GraphicsEngine::Get().RenderQuad();
}

void RenderFullscreenQuad::Destroy()
{
}
