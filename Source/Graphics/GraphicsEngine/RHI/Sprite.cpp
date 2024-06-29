#include "GraphicsEngine.pch.h"
#include "Graphics\GraphicsEngine\GraphicsEngine.h"
#include "Sprite.h"
#include "SpriteVertex.h"

using namespace Microsoft::WRL;

Sprite::Sprite()
{
    myVertices.push_back(SpriteVertex());
    GraphicsEngine::Get().CreateVertexBuffer("Vertex Buffer", myVertices, myVertexBuffer);
}

Sprite::~Sprite()
{
    myVertices.clear();
    myVertexBuffer.ReleaseAndGetAddressOf();
}