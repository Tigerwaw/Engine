#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

struct RenderFullscreenQuad : GraphicsCommandBase
{
public:
	RenderFullscreenQuad();
	void Execute() override;
	void Destroy() override;
};