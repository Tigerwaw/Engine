#include "Assetpch.h"
#include "MaterialAsset.h"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;
#include <fstream>
#include "AssetManager.h"
#include "PSOAsset.h"
#include "TextureAsset.h"

bool MaterialAsset::Load()
{
    std::ifstream streamPath(GetPath());
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(streamPath);
    }
    catch (nl::json::parse_error)
    {
        return false;
    }
    streamPath.close();

    Material mat;

    if (data.contains("PSO"))
    {
        mat.SetPSO(AssetManager::Get().GetAsset<PSOAsset>(data["PSO"].get<std::string>())->pso);
    }

    if (data.contains("AlbedoTint"))
    {
        mat.MaterialSettings().albedoTint = { data["AlbedoTint"]["R"].get<float>(),
                                                    data["AlbedoTint"]["G"].get<float>(),
                                                    data["AlbedoTint"]["B"].get<float>(),
                                                    data["AlbedoTint"]["A"].get<float>() };
    }

    if (data.contains("EmissiveStrength"))
    {
        mat.MaterialSettings().emissiveStrength = data["EmissiveStrength"].get<float>();
    }

    if (data.contains("Textures"))
    {
        unsigned textureIndex = 0;
        for (auto& texturePath : data["Textures"])
        {
            std::filesystem::path texPath = texturePath.get<std::string>();
            std::string texNameLower = Utilities::ToLowerCopy(texPath.filename().string());
            std::filesystem::path texName(texNameLower);
            if (auto texAsset = AssetManager::Get().GetAsset<TextureAsset>(texName))
            {
                mat.SetTextureOnSlot(textureIndex, texAsset->texture);
            }
            else
            {
                return false;
            }

            ++textureIndex;
        }
    }

    material = std::make_shared<Material>(std::move(mat));
    return true;
}

bool MaterialAsset::Unload()
{
    material = nullptr;
    return true;
}
