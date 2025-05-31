#include "GraphicsEngine.pch.h"
#include "RenderSprite.h"

#include "GraphicsEngine.h"
#include "Objects/ConstantBuffers/SpriteBuffer.h"
#include "Objects/Sprite.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "AssetManager.h"

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
        GraphicsEngine::Get().SetTextureResource_PS(0, material->GetTexture(Material::TextureType::Albedo));
        GraphicsEngine::Get().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
    else
    {
        GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("PSO_Sprite")->pso);
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
