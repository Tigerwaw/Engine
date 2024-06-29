#include "GraphicsEngine.pch.h"
#include "RenderUISprite.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/ConstantBuffers/SpriteBuffer.h"
#include "GameEngine/UI/SpriteObject.h"
#include "GraphicsEngine/RHI/Sprite.h"
#include "GraphicsEngine/RHI/Texture.h"

RenderUISprite::RenderUISprite(std::shared_ptr<SpriteObject> aSpriteObject)
{
    sprite = aSpriteObject->GetSprite();
    texture = aSpriteObject->GetTexture();
    position = CU::ToVector4(aSpriteObject->GetPosition());
    size = aSpriteObject->GetSize();
}

void RenderUISprite::Execute()
{
    if (!sprite) return;
    if (!texture) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Position = position;
    spriteBufferData.Size = size;
    spriteBufferData.IsScreenSpace = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::SpriteBuffer, spriteBufferData);

    GraphicsEngine::Get().SetTextureResource_PS(0, *texture);
    GraphicsEngine::Get().RenderSprite(*sprite);
    GraphicsEngine::Get().ClearTextureResource_PS(0);
}

void RenderUISprite::Destroy()
{
    sprite = nullptr;
    texture = nullptr;
}
