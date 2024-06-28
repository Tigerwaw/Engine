#include "GraphicsEngine.pch.h"
#include "ClearTextureResource.h"

ClearTextureResource::ClearTextureResource(unsigned aSlot)
{
    mySlot = aSlot;
}

void ClearTextureResource::Execute()
{
    GraphicsEngine::Get().ClearTextureResource_PS(mySlot);
    GraphicsEngine::Get().RenderSprite();
}

void ClearTextureResource::Destroy()
{
}
