#include "GraphicsEngine.pch.h"
#include "DebugLineVertex.h"

const std::vector<VertexElementDesc> DebugLineVertex::InputLayoutDefinition =
{
	{ "FROM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ "TO", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT }
};