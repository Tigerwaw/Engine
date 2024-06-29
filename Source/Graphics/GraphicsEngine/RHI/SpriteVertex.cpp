#include "GraphicsEngine.pch.h"
#include "SpriteVertex.h"

const std::vector<VertexElementDesc> SpriteVertex::InputLayoutDefinition =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT },
    { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT }
};
