#include "Assetpch.h"
#include "ShaderAsset.h"
#include "GraphicsEngine.h"

bool ShaderAsset::Load()
{
    Shader sh;
    if (!GraphicsEngine::Get().GetResourceVendor().LoadShader(GetPath(), sh))
    {
        return false;
    }

    shader = std::make_shared<Shader>(std::move(sh));
    return true;
}

bool ShaderAsset::Unload()
{
    shader = nullptr;
    return true;
}
