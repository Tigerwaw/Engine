#include "Assetpch.h"
#include "PSOAsset.h"
#include "GraphicsEngine.h"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;
#include <fstream>
#include "AssetManager.h"
#include "ShaderAsset.h"
#include "Objects/Vertices/Vertex.h"
#include "Objects/Vertices/DebugLineVertex.h"
#include "Objects/Vertices/TextVertex.h"
#include "Objects/Vertices/ParticleVertex.h"
#include "Objects/Vertices/TrailVertex.h"

bool PSOAsset::Load()
{
    std::ifstream streamPath(GetPath());
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(streamPath);
    }
    catch (nl::json::parse_error e)
    {
        return false;
    }
    streamPath.close();

    PSODescription psoDesc = {};

    psoDesc.name = GetName().string();

    if (data.contains("VertexType"))
    {
        if (data["VertexType"].get<std::string>() == "Default")
        {
            psoDesc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
            psoDesc.vertexStride = sizeof(Vertex);
        }
        else if (data["VertexType"].get<std::string>() == "DebugLine")
        {
            psoDesc.inputLayoutDefinition = DebugLineVertex::InputLayoutDefinition;
            psoDesc.vertexStride = sizeof(DebugLineVertex);
        }
        else if (data["VertexType"].get<std::string>() == "Text")
        {
            psoDesc.inputLayoutDefinition = TextVertex::InputLayoutDefinition;
            psoDesc.vertexStride = sizeof(TextVertex);
        }
        else if (data["VertexType"].get<std::string>() == "Particle")
        {
            psoDesc.inputLayoutDefinition = ParticleVertex::InputLayoutDefinition;
            psoDesc.vertexStride = sizeof(ParticleVertex);
        }
        else if (data["VertexType"].get<std::string>() == "Trail")
        {
            psoDesc.inputLayoutDefinition = TrailVertex::InputLayoutDefinition;
            psoDesc.vertexStride = sizeof(TrailVertex);
        }
    }

    std::filesystem::path vsPath = "";
    std::filesystem::path gsPath = "";
    std::filesystem::path psPath = "";
    std::shared_ptr<ShaderAsset> vsShaderAsset;
    std::shared_ptr<ShaderAsset> gsShaderAsset;
    std::shared_ptr<ShaderAsset> psShaderAsset;

    auto& assetManager = AssetManager::Get();

    if (data.contains("VertexShader") && data["VertexShader"] != "")
    {
        vsShaderAsset = assetManager.GetAsset<ShaderAsset>(data["VertexShader"].get<std::string>());
        if (vsShaderAsset)
        {
            vsPath = vsShaderAsset->GetPath();
        }
    }

    if (data.contains("GeometryShader") && data["GeometryShader"] != "")
    {
        gsShaderAsset = assetManager.GetAsset<ShaderAsset>(data["GeometryShader"].get<std::string>());
        if (gsShaderAsset)
        {
            gsPath = gsShaderAsset->GetPath();
        }
    }

    if (data.contains("PixelShader") && data["PixelShader"] != "")
    {
        psShaderAsset = assetManager.GetAsset<ShaderAsset>(data["PixelShader"].get<std::string>());
        if (psShaderAsset)
        {
            psPath = psShaderAsset->GetPath();
        }
    }

#ifndef _RETAIL
    if (!GraphicsEngine::Get().GetResourceVendor().ValidateShaderCombination(vsPath, gsPath, psPath))
    {
        return false;
    }
#endif


    if (vsShaderAsset)
    {
        psoDesc.vsPath = vsPath.wstring();
        psoDesc.vsShader = vsShaderAsset->shader;
    }

    if (gsShaderAsset)
    {
        psoDesc.gsShader = gsShaderAsset->shader;
    }

    if (psShaderAsset)
    {
        psoDesc.psShader = psShaderAsset->shader;
    }

    if (data.contains("RasterizerDesc"))
    {
        if (data["RasterizerDesc"].contains("FillMode"))
        {
            std::string fillMode = data["RasterizerDesc"]["FillMode"].get<std::string>();
            if (fillMode == "Wireframe" || fillMode == "wireframe")
            {
                psoDesc.fillMode = 2;
            }
            else
            {
                psoDesc.fillMode = 3;
            }
        }

        if (data["RasterizerDesc"].contains("CullMode"))
        {
            std::string cullMode = data["RasterizerDesc"]["CullMode"].get<std::string>();
            if (cullMode == "None")
            {
                psoDesc.cullMode = 1;
            }
            else if (cullMode == "Front")
            {
                psoDesc.cullMode = 2;
            }
            else
            {
                psoDesc.cullMode = 3;
            }
        }

        if (data["RasterizerDesc"].contains("AntialiasedLine"))
        {
            psoDesc.antiAliasedLine = data["RasterizerDesc"]["AntialiasedLine"].get<bool>();
        }
    }

    if (data.contains("BlendStateDesc"))
    {
        if (data["BlendStateDesc"].contains("BlendMode"))
        {
            std::string blendMode = data["BlendStateDesc"]["BlendMode"].get<std::string>();
            if (blendMode == "Alpha")
            {
                psoDesc.blendMode = BlendMode::Alpha;
            }
            else if (blendMode == "Additive")
            {
                psoDesc.blendMode = BlendMode::Additive;
            }
            else
            {
                psoDesc.blendMode = BlendMode::None;
            }
        }

        if (data["BlendStateDesc"].contains("AlphaToCoverage"))
        {
            psoDesc.alphaToCoverage = data["BlendStateDesc"]["AlphaToCoverage"].get<bool>();
        }

        if (data["BlendStateDesc"].contains("IndependentBlend"))
        {
            psoDesc.independentBlend = data["BlendStateDesc"]["IndependentBlend"].get<bool>();
        }
    }

    if (data.contains("UseReadOnlyDepthStencil"))
    {
        psoDesc.useReadOnlyDepthStencilState = data["UseReadOnlyDepthStencil"].get<bool>();
    }

    if (data.contains("Samplers"))
    {
        for (auto& sampler : data["Samplers"].items())
        {
            psoDesc.samplerList.emplace(sampler.value().get<unsigned>(), sampler.key());
        }
    }

    pso = std::make_shared<PipelineStateObject>();
    if (!GraphicsEngine::Get().GetResourceVendor().CreatePSO(*pso, psoDesc))
    {
        pso = nullptr;
        return false;
    }

    return true;
}

bool PSOAsset::Unload()
{
    pso = nullptr;
    return true;
}
