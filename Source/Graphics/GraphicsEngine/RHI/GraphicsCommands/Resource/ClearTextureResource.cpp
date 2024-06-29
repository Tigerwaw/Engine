#include "GraphicsEngine.pch.h"
#include "ClearTextureResource.h"

ClearTextureResource::ClearTextureResource(unsigned aSlot)
{
    mySlot = aSlot;
}

void ClearTextureResource::Execute()
{
    GraphicsEngine::Get().ClearTextureResource_PS(mySlot);
}

void ClearTextureResource::Destroy()
{
}
