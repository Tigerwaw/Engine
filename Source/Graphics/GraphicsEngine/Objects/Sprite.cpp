#include "GraphicsEngine.pch.h"
#include "Sprite.h"

#include "GraphicsEngine/Objects/Texture.h"
#include "GraphicsEngine/Objects/Material.h"

Sprite::Sprite(CU::Vector2f aPosition, CU::Vector2f aSize)
{
    SetPosition(aPosition);
    SetSize(aSize);
}

Sprite::~Sprite()
{
    myTexture = nullptr;
}

void Sprite::SetMaterial(std::shared_ptr<Material> aMaterial)
{
    myMaterial = aMaterial;
}

void Sprite::SetTexture(std::shared_ptr<Texture> aTexture)
{
    myTexture = aTexture;
}

void Sprite::SetPosition(CU::Vector2f aPosition)
{
    myMatrix(4, 1) = aPosition.x;
    myMatrix(4, 2) = aPosition.y;
}

void Sprite::SetSize(CU::Vector2f aSize)
{
    myMatrix(1, 1) = aSize.x;
    myMatrix(2, 2) = aSize.y;
}
