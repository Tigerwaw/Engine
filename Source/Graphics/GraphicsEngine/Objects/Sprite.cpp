#include "GraphicsEngine.pch.h"
#include "Sprite.h"

#include "GraphicsEngine/Objects/Texture.h"
#include "GraphicsEngine/Objects/Material.h"

Sprite::Sprite(CU::Vector2f aPosition, CU::Vector2f aSize)
{
    SetScreenspacePosition(aPosition);
    SetScreenspaceSize(aSize);
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

void Sprite::SetScreenspacePosition(CU::Vector2f aPosition)
{
    myScreenspaceMatrix(4, 1) = aPosition.x;
    myScreenspaceMatrix(4, 2) = aPosition.y;
}

void Sprite::SetScreenspaceSize(CU::Vector2f aSize)
{
    myScreenspaceMatrix(1, 1) = aSize.x;
    myScreenspaceMatrix(2, 2) = aSize.y;
}

void Sprite::SetIsScreenspace(bool aIsScreenspace)
{
    myIsScreenspace = aIsScreenspace;
}
