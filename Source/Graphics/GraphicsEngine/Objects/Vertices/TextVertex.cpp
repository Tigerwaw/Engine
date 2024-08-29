#include "GraphicsEngine.pch.h"
#include "TextVertex.h"

const std::vector<VertexElementDesc> TextVertex::InputLayoutDefinition =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT },
};