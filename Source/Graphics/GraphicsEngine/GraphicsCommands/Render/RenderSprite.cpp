#include "GraphicsEngine.pch.h"
#include "RenderSprite.h"

#include "GraphicsEngine.h"
#include "Objects/ConstantBuffers/SpriteBuffer.h"
#include "Objects/Sprite.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"

RenderSprite::RenderSprite(const SpriteData& aSpriteData)
{
    myData = aSpriteData;
    //material = aSprite->GetMaterial();
    //texture = aSprite->GetTexture();
    //matrix = aSprite->GetMatrix();
}

void RenderSprite::Execute()
{
    if (!myData.texture && !myData.material) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Matrix = myData.matrix;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::SpriteBuffer, spriteBufferData);

    if (myData.material)
    {
        GraphicsEngine::Get().ChangePipelineState(myData.material->GetPSO());
        GraphicsEngine::Get().SetTextureResource_PS(0, myData.material->GetTexture(Material::TextureType::Albedo));
        GraphicsEngine::Get().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
    else
    {
        GraphicsEngine::Get().SetTextureResource_PS(0, *myData.texture);
        GraphicsEngine::Get().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
}

void RenderSprite::Destroy()
{
    myData.material = nullptr;
    myData.texture = nullptr;
}
