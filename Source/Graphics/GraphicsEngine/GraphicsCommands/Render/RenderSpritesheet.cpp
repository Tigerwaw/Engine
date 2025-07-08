#include "GraphicsEngine.pch.h"
#include "RenderSpritesheet.h"

#include "GraphicsEngine.h"
#include "Objects/ConstantBuffers/SpriteBuffer.h"
#include "Objects/Spritesheet.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"

RenderSpritesheet::RenderSpritesheet(const SpritesheetData& aSpriteData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSpritesheet Copy Constructor");
    myData = aSpriteData;
}

RenderSpritesheet::RenderSpritesheet(SpritesheetData&& aSpriteData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSpritesheet Move Constructor");
    myData = std::move(aSpriteData);
}

void RenderSpritesheet::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSpritesheet Execute");
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
