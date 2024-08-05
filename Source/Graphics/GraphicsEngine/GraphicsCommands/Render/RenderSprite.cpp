#include "GraphicsEngine.pch.h"
#include "RenderSprite.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/ConstantBuffers/SpriteBuffer.h"
#include "GraphicsEngine/Objects/Sprite.h"
#include "GraphicsEngine/Objects/Texture.h"
#include "GraphicsEngine/Objects/Material.h"
#include "AssetManager/AssetManager.h"

RenderSprite::RenderSprite(std::shared_ptr<Sprite> aSprite)
{
    material = aSprite->GetMaterial();
    texture = aSprite->GetTexture();
    matrix = aSprite->GetMatrix();
}

void RenderSprite::Execute()
{
    if (!texture && !material) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Matrix = matrix;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::SpriteBuffer, spriteBufferData);

    if (material)
    {
        GraphicsEngine::Get().ChangePipelineState(material->GetPSO());
        GraphicsEngine::Get().SetTextureResource_PS(0, material->GetAlbedoTexture());
        GraphicsEngine::Get().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
    else
    {
        GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("Sprite")->pso);
        GraphicsEngine::Get().SetTextureResource_PS(0, *texture);
        GraphicsEngine::Get().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
}

void RenderSprite::Destroy()
{
    material = nullptr;
    texture = nullptr;
}
