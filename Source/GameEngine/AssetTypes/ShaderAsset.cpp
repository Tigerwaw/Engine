#include "Enginepch.h"
#include "ShaderAsset.h"
#include "GraphicsEngine.h"

bool ShaderAsset::Load()
{
    if (GraphicsEngine::Get().IsShaderCached(GetPath()))
    {
        shader = GraphicsEngine::Get().GetCachedShader(GetPath());
    }
    else
    {
        Shader sh;
        if (!GraphicsEngine::Get().GetResourceVendor().LoadShader(GetPath(), sh))
        {
            return false;
        }

        shader = std::make_shared<Shader>(std::move(sh));
    }

    return true;
}

bool ShaderAsset::Unload()
{
    shader = nullptr;
    return true;
}
