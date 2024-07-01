#include "GraphicsEngine.pch.h"
#include "Sprite.h"

Sprite::Sprite(CU::Vector3f aPosition, CU::Vector2f aSize)
{
    myPosition = aPosition;
    mySize = aSize;
}

Sprite::~Sprite()
{
    myTexture = nullptr;
}

void Sprite::SetTexture(std::shared_ptr<Texture> aTexture)
{
    myTexture = aTexture;
}

void Sprite::SetPosition(CU::Vector3f aPosition)
{
    myPosition = aPosition;
}

void Sprite::SetSize(CU::Vector2f aSize)
{
    mySize = aSize;
}