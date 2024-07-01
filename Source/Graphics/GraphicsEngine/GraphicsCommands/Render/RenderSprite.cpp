#include "GraphicsEngine.pch.h"
#include "RenderSprite.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/ConstantBuffers/SpriteBuffer.h"
#include "GraphicsEngine/Objects/Sprite.h"
#include "GraphicsEngine/Objects/Texture.h"

RenderSprite::RenderSprite(std::shared_ptr<Sprite> aSprite)
{
    texture = aSprite->GetTexture();
    position = CU::ToVector4(aSprite->GetPosition());
    size = aSprite->GetSize();
}

void RenderSprite::Execute()
{
    if (!texture) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Position = position;
    spriteBufferData.Size = size;
    spriteBufferData.IsScreenSpace = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::SpriteBuffer, spriteBufferData);

    GraphicsEngine::Get().SetTextureResource_PS(0, *texture);
    GraphicsEngine::Get().RenderSprite();
    GraphicsEngine::Get().ClearTextureResource_PS(0);
}

void RenderSprite::Destroy()
{
    texture = nullptr;
}
