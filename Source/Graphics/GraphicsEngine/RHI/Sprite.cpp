#include "GraphicsEngine.pch.h"
#include "Graphics\GraphicsEngine\GraphicsEngine.h"
#include "Sprite.h"
#include "SpriteVertex.h"

using namespace Microsoft::WRL;

Sprite::Sprite() = default;
Sprite::~Sprite() = default;

void Sprite::Initialize(CU::Vector3f aPosition, CU::Vector2f aSize)
{
    float pos[4] = { aPosition.x, aPosition.y, aPosition.z, 1.0f };
    float size[2] = { aSize.x, aSize.y };
    myVertices.push_back(SpriteVertex(pos, size));
    GraphicsEngine::Get().CreateVertexBuffer("Vertex Buffer", myVertices, myVertexBuffer);
}
