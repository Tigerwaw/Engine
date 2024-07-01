#include "GraphicsEngine.pch.h"
#include "SetTextureResource.h"

#include "GraphicsEngine/Objects/Texture.h"

SetTextureResource::SetTextureResource(unsigned aSlot, std::shared_ptr<Texture> aTexture)
{
    mySlot = aSlot;
    myTexture = aTexture;
}

void SetTextureResource::Execute()
{
    GraphicsEngine::Get().SetTextureResource_PS(mySlot, *myTexture);
}

void SetTextureResource::Destroy()
{
    myTexture = nullptr;
}
