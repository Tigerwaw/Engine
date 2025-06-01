#include "GraphicsEngine.pch.h"
#include "RenderSpritesheet.h"

#include "GraphicsEngine.h"
#include "Objects/ConstantBuffers/SpriteBuffer.h"
#include "Objects/Spritesheet.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"

RenderSpritesheet::RenderSpritesheet(const SpritesheetData& aSpriteData)
{
    myData = aSpriteData;
    //material = aSpritesheet->GetMaterial();
    //texture = aSpritesheet->GetTexture();
    //matrix = aSpritesheet->GetMatrix();
    //sheetDimensions = { static_cast<float>(aSpritesheet->GetSheetDimensions().x), static_cast<float>(aSpritesheet->GetSheetDimensions().y) };
    //currentFrame = static_cast<float>(aSpritesheet->GetCurrentFrame());
}

void RenderSpritesheet::Execute()
{
    if (!myData.texture && !myData.material) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Matrix = myData.matrix;
    spriteBufferData.CurrentFrame = myData.currentFrame;
    spriteBufferData.SpriteSheetDimensions = myData.sheetDimensions;
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

void RenderSpritesheet::Destroy()
{
    myData.material = nullptr;
    myData.texture = nullptr;
}
