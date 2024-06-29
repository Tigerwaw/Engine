#include "SpriteObject.h"
#include "Graphics/GraphicsEngine/RHI/Sprite.h"

SpriteObject::SpriteObject(CU::Vector3f aPosition, CU::Vector2f aSize)
{
    mySprite = std::make_shared<Sprite>();
    myPosition = aPosition;
    mySize = aSize;
}

SpriteObject::~SpriteObject()
{
    mySprite = nullptr;
    myTexture = nullptr;
}

void SpriteObject::SetTexture(std::shared_ptr<Texture> aTexture)
{
    myTexture = aTexture;
}

void SpriteObject::SetPosition(CU::Vector3f aPosition)
{
    myPosition = aPosition;
}

void SpriteObject::SetSize(CU::Vector2f aSize)
{
    mySize = aSize;
}
