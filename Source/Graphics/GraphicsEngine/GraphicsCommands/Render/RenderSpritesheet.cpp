#include "GraphicsEngine.pch.h"
#include "RenderSpritesheet.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/ConstantBuffers/SpriteBuffer.h"
#include "GraphicsEngine/Objects/Spritesheet.h"
#include "GraphicsEngine/Objects/Texture.h"
#include "GraphicsEngine/Objects/Material.h"
#include "AssetManager/AssetManager.h"

RenderSpritesheet::RenderSpritesheet(std::shared_ptr<Spritesheet> aSpritesheet)
{
    material = aSpritesheet->GetMaterial();
    texture = aSpritesheet->GetTexture();
    matrix = aSpritesheet->GetMatrix();
    sheetDimensions = { static_cast<float>(aSpritesheet->GetSheetDimensions().x), static_cast<float>(aSpritesheet->GetSheetDimensions().y) };
    currentFrame = static_cast<float>(aSpritesheet->GetCurrentFrame());
}

void RenderSpritesheet::Execute()
{
    if (!texture && !material) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Matrix = matrix;
    spriteBufferData.CurrentFrame = currentFrame;
    spriteBufferData.SpriteSheetDimensions = sheetDimensions;
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
        GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("Spritesheet")->pso);
        GraphicsEngine::Get().SetTextureResource_PS(0, *texture);
        GraphicsEngine::Get().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
}

void RenderSpritesheet::Destroy()
{
    material = nullptr;
    texture = nullptr;
}
