#include "GraphicsEngine.pch.h"
#include "TrailVertex.h"

const std::vector<VertexElementDesc> TrailVertex::InputLayoutDefinition =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT },
	{ "WIDTH", 0, DXGI_FORMAT_R32_FLOAT },
	{ "CHANNELMASK", 0, DXGI_FORMAT_R32G32B32A32_FLOAT }
};