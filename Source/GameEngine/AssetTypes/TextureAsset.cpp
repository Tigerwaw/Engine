#include "Enginepch.h"
#include "TextureAsset.h"
#include "GraphicsEngine.h"

bool TextureAsset::Load()
{
    Texture tex;
    if (!GraphicsEngine::Get().GetResourceVendor().LoadTexture(GetPath(), tex))
    {
        return false;
    }

    texture = std::make_shared<Texture>(std::move(tex));
    return true;
}

bool TextureAsset::Unload()
{
    texture = nullptr;
    return true;
}
