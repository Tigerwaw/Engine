#include "Assetpch.h"

#include "AssetManager.h"
#include "Asset.h"
#include "GraphicsEngine.h"
#include "Objects/Vertices/Vertex.h"
#include "Objects/Vertices/DebugLineVertex.h"
#include "Objects/Vertices/TextVertex.h"
#include "Objects/Vertices/ParticleVertex.h"
#include "Objects/Vertices/TrailVertex.h"

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
namespace CU = CommonUtilities;

#include "DefaultTextures/Default_C.h"
#include "DefaultTextures/Default_N.h"
#include "DefaultTextures/Default_M.h"
#include "DefaultTextures/Default_FX.h"

bool AssetManager::UnregisterAsset(const std::filesystem::path& aPath)
{
    if (!myAssets.contains(aPath))
    {
        LOG(LogAssetManager, Warning, "Couldn't unregister asset at path {} since it does not seem to exist!", aPath.string());
        return false;
    }

    myAssets.erase(aPath);
    LOG(LogAssetManager, Log, "Successfully unregistered asset at path {}!", aPath.string());
    return true;
}

bool AssetManager::UnregisterAsset(const std::shared_ptr<Asset> aAsset)
{
    return UnregisterAsset(aAsset->path);
}

// Handle case sensitivity betterer
bool AssetManager::RegisterAsset(const std::filesystem::path& aPath)
{
    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string name = assetPath.filename().string();
    if (name.starts_with("SM") || name.starts_with("sm") || name.starts_with("SK") || name.starts_with("sk"))
    {
        return RegisterMeshAsset(aPath);
    }
    else if (name.starts_with("A") || name.starts_with("a"))
    {
        return RegisterAnimationAsset(aPath);
    }
    else if (name.starts_with("MAT") || name.starts_with("mat"))
    {
        return RegisterMaterialAsset(aPath);
    }
    else if (name.starts_with("T") || name.starts_with("t"))
    {
        return RegisterTextureAsset(aPath);
    }
    else if (name.starts_with("SH") || name.starts_with("sh"))
    {
        return RegisterShaderAsset(aPath);
    }
    else if (name.starts_with("PSO") || name.starts_with("pso"))
    {
        return RegisterPSOAsset(aPath);
    }
    else if (name.starts_with("F") || name.starts_with("f"))
    {
        return RegisterFontAsset(aPath);
    }
    else if (name.starts_with("NM") || name.starts_with("nm"))
    {
        return RegisterNavMeshAsset(aPath);
    }

    return false;
}

bool AssetManager::Initialize(const std::filesystem::path& aContentRootPath, bool aAutoRegisterAllAssetsInRoot)
{
    LOG(LogAssetManager, Log, "Initializing Asset Manager...");

    myContentRoot = std::filesystem::absolute(aContentRootPath);
    if (!std::filesystem::exists(aContentRootPath))
    {
        LOG(LogAssetManager, Error, "Can't find directory at path '{}'! Trying to create a new directory!", aContentRootPath.string());

        if (!std::filesystem::create_directories(aContentRootPath))
        {
            LOG(LogAssetManager, Error, "Failed to initialize Asset Manager! Can't find or create directory at path '{}'!", aContentRootPath.string());
            return false;
        }
    }

    RegisterEngineAssets();

    if (aAutoRegisterAllAssetsInRoot)
    {
        RegisterAllAssetsInDirectory();
    }

    LOG(LogAssetManager, Log, "Asset Manager Initialized! Root content directory set to '{}'.", myContentRoot.string());
    return true;
}

void AssetManager::RegisterEngineAssets()
{
    RegisterEngineTextureAsset("T_Default_C", BuiltIn_Default_C_ByteCode, sizeof(BuiltIn_Default_C_ByteCode));
    RegisterEngineTextureAsset("T_Default_N", BuiltIn_Default_N_ByteCode, sizeof(BuiltIn_Default_N_ByteCode));
    RegisterEngineTextureAsset("T_Default_M", BuiltIn_Default_M_ByteCode, sizeof(BuiltIn_Default_M_ByteCode));
    RegisterEngineTextureAsset("T_Default_FX", BuiltIn_Default_FX_ByteCode, sizeof(BuiltIn_Default_FX_ByteCode));

    std::shared_ptr<MaterialAsset> asset = std::make_shared<MaterialAsset>();
    asset->material = std::make_shared<Material>();
    asset->material->SetPSO(GraphicsEngine::Get().GetDefaultPSO());
    asset->material->MaterialSettings().albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
    asset->material->MaterialSettings().emissiveStrength = 0.0f;
    asset->material->SetTexture(Material::TextureType::Albedo, GetAsset<TextureAsset>("T_Default_C")->texture);
    asset->material->SetTexture(Material::TextureType::Normal, GetAsset<TextureAsset>("T_Default_N")->texture);
    asset->material->SetTexture(Material::TextureType::Material, GetAsset<TextureAsset>("T_Default_M")->texture);
    asset->material->SetTexture(Material::TextureType::Effects, GetAsset<TextureAsset>("T_Default_FX")->texture);
    asset->name = "MAT_DefaultMaterial";
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered material asset {}", asset->name.string());

    RegisterPlanePrimitive();
    RegisterCubePrimitive();
    RegisterRampPrimitive();

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Models/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterMeshAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Textures/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterTextureAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Shaders/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterShaderAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/PSOs/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterPSOAsset(file.path());
        }
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Materials/"))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterMaterialAsset(file.path());
        }
    }
}

void AssetManager::RegisterAllAssetsInDirectory()
{
    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            RegisterAsset(file.path());
        }
    }
}

bool AssetManager::RegisterMeshAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::Mesh tgaMesh;
    TGA::FBX::Importer::LoadMesh(aPath, tgaMesh);

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Mesh::Element> elements;

    CU::Vector3<float> minBBPoint;
    CU::Vector3<float> maxBBPoint;

    unsigned nextVertexOffset = 0;
    unsigned nextIndexOffset = 0;

    for (auto& tgaElement : tgaMesh.Elements)
    {
        Mesh::Element element;
        element.VertexOffset = nextVertexOffset;
        element.IndexOffset = nextIndexOffset;
        element.NumVertices = static_cast<int>(tgaElement.Vertices.size());
        element.NumIndices = static_cast<int>(tgaElement.Indices.size());
        element.MaterialIndex = tgaElement.MaterialIndex;
        elements.push_back(element);

        for (auto& v : tgaElement.Vertices)
        {
            vertices.push_back(Vertex(v.Position, v.VertexColors, v.BoneIDs, v.BoneWeights, v.UVs, v.Normal, v.Tangent));
            nextVertexOffset++;

            minBBPoint.x = v.Position[0] < minBBPoint.x ? v.Position[0] : minBBPoint.x;
            minBBPoint.y = v.Position[1] < minBBPoint.y ? v.Position[1] : minBBPoint.y;
            minBBPoint.z = v.Position[2] < minBBPoint.z ? v.Position[2] : minBBPoint.z;

            maxBBPoint.x = v.Position[0] > maxBBPoint.x ? v.Position[0] : maxBBPoint.x;
            maxBBPoint.y = v.Position[1] > maxBBPoint.y ? v.Position[1] : maxBBPoint.y;
            maxBBPoint.z = v.Position[2] > maxBBPoint.z ? v.Position[2] : maxBBPoint.z;
        }

        for (auto& i : tgaElement.Indices)
        {
            indices.push_back(i + element.VertexOffset);
            nextIndexOffset++;
        }
    }

    Mesh::Skeleton skeleton;

    for (auto& tgaJoint : tgaMesh.Skeleton.Bones)
    {
        Mesh::Skeleton::Joint& joint = skeleton.myJoints.emplace_back(Mesh::Skeleton::Joint());
        joint.Parent = tgaJoint.ParentIdx;
        joint.Children = tgaJoint.Children;
        joint.Name = tgaJoint.Name;
        auto& matrix = tgaJoint.BindPoseInverse.Data;
        joint.BindPoseInverse = { matrix[0], matrix[1], matrix[2], matrix[3],
                                  matrix[4], matrix[5], matrix[6], matrix[7],
                                  matrix[8], matrix[9], matrix[10], matrix[11],
                                  matrix[12], matrix[13], matrix[14], matrix[15] };

        joint.BindPoseInverse = joint.BindPoseInverse.GetTranspose();
    }

    skeleton.JointNameToIndex = tgaMesh.Skeleton.BoneNameToIndex;

    Mesh mesh;
    mesh.InitBoundingBox(minBBPoint, maxBBPoint);
    mesh.Initialize(std::move(vertices), std::move(indices), std::move(elements), std::move(skeleton));

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(mesh));
    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterAnimationAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::Animation tgaAnimation;
    TGA::FBX::Importer::LoadAnimation(aPath, tgaAnimation);

    Animation animation;
    animation.Duration = static_cast<float>(tgaAnimation.Duration);
    animation.FramesPerSecond = tgaAnimation.FramesPerSecond;

    for (auto& tgaAnimFrame : tgaAnimation.Frames)
    {
        Animation::Frame frame;
        for (auto& tgaAnimFrameJoint : tgaAnimFrame.LocalTransforms)
        {
            auto& matrix = tgaAnimFrameJoint.second.Data;
            CommonUtilities::Matrix4x4<float> jointTransform = { matrix[0], matrix[1], matrix[2], matrix[3],
                                                                 matrix[4], matrix[5], matrix[6], matrix[7],
                                                                 matrix[8], matrix[9], matrix[10], matrix[11],
                                                                 matrix[12], matrix[13], matrix[14], matrix[15] };
            frame.BoneTransforms.emplace(tgaAnimFrameJoint.first, jointTransform);
        }

        animation.Frames.emplace_back(frame);
    }

    std::shared_ptr<AnimationAsset> asset = std::make_shared<AnimationAsset>();
    asset->animation = std::make_shared<Animation>(std::move(animation));
    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered animation asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterMaterialAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("json")) return false;
    //if (!ext.ends_with("mat")) return false;

    std::shared_ptr<MaterialAsset> asset = std::make_shared<MaterialAsset>();
    asset->material = std::make_shared<Material>();
    asset->material->SetPSO(GraphicsEngine::Get().GetDefaultPSO());
    asset->material->SetTexture(Material::TextureType::Albedo, GetAsset<TextureAsset>("T_Default_C")->texture);
    asset->material->SetTexture(Material::TextureType::Normal, GetAsset<TextureAsset>("T_Default_N")->texture);
    asset->material->SetTexture(Material::TextureType::Material, GetAsset<TextureAsset>("T_Default_M")->texture);
    asset->material->SetTexture(Material::TextureType::Effects, GetAsset<TextureAsset>("T_Default_FX")->texture);

    std::ifstream path(aPath);
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        LOG(LogAssetManager, Error, "Failed to read material asset {}, {}", asset->path.filename().string(), e.what());
        return false;
    }
    path.close();

    if (data.contains("PSO"))
    {
        asset->material->SetPSO(GetAsset<PSOAsset>(data["PSO"].get<std::string>())->pso);
    }

    if (data.contains("AlbedoTint"))
    {
        asset->material->MaterialSettings().albedoTint = { data["AlbedoTint"]["R"].get<float>(),
                                                           data["AlbedoTint"]["G"].get<float>(),
                                                           data["AlbedoTint"]["B"].get<float>(),
                                                           data["AlbedoTint"]["A"].get<float>() };
    }

    if (data.contains("EmissiveStrength"))
    {
        asset->material->MaterialSettings().emissiveStrength = data["EmissiveStrength"].get<float>();
    }

    if (data.contains("Textures"))
    {
        unsigned textureIndex = 0;
        for (auto& texturePath : data["Textures"])
        {
            bool textureExists = true;
            std::filesystem::path texPath = texturePath.get<std::string>();
            std::filesystem::path texName = texPath.stem();
            if (myAssets.find(texName) == myAssets.end())
            {
                textureExists = RegisterTextureAsset(myContentRoot / texPath);
            }

            if (textureExists)
            {
                asset->material->SetTextureOnSlot(textureIndex, GetAsset<TextureAsset>(texName)->texture);
            }

            textureIndex++;
        }
    }

    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered material asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterTextureAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;
    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("dds")) return false;

    std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>();
    asset->texture = std::make_shared<Texture>();
    std::filesystem::path absolutePath = aPath;
    if (!GraphicsEngine::Get().LoadTexture(absolutePath, *asset->texture))
    {
        LOG(LogAssetManager, Error, "Failed to register texture asset {}", assetPath.string());
        return false;
    }
    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered texture asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterShaderAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;
    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("cso")) return false;

    std::shared_ptr<ShaderAsset> asset = std::make_shared<ShaderAsset>();
    asset->shader = std::make_shared<Shader>();
    std::filesystem::path absolutePath = aPath;
    if (!GraphicsEngine::Get().LoadShader(absolutePath, *asset->shader))
    {
        LOG(LogAssetManager, Error, "Failed to register shader asset {}", assetPath.string());
        return false;
    }

    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered shader asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterPSOAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("json")) return false;

    std::shared_ptr<PSOAsset> asset = std::make_shared<PSOAsset>();

    std::ifstream path(aPath);
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        LOG(LogAssetManager, Error, "Failed to read pso asset {}, {}", asset->path.filename().string(), e.what());
        return false;
    }
    path.close();

    PSODescription psoDesc = {};
    
    psoDesc.name = assetPath.stem().string();

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

    if (data.contains("VertexShader") && data["VertexShader"] != "")
    {
        vsPath = GetContentRoot() / GetAsset<ShaderAsset>(data["VertexShader"].get<std::string>())->path;
    }

    if (data.contains("GeometryShader") && data["GeometryShader"] != "")
    {
        gsPath = GetContentRoot() / GetAsset<ShaderAsset>(data["GeometryShader"].get<std::string>())->path;
    }

    if (data.contains("PixelShader") && data["PixelShader"] != "")
    {
        psPath = GetContentRoot() / GetAsset<ShaderAsset>(data["PixelShader"].get<std::string>())->path;
    }

#ifndef _RETAIL
    if (!GraphicsEngine::Get().ValidateShaderCombination(vsPath, gsPath, psPath))
    {
        return false;
    }
#endif


    if (!vsPath.empty())
    {
        psoDesc.vsPath = vsPath.wstring();
        psoDesc.vsShader = GetAsset<ShaderAsset>(vsPath.stem())->shader;
    }

    if (!gsPath.empty())
    {
        psoDesc.gsShader = GetAsset<ShaderAsset>(gsPath.stem())->shader;
    }

    if (!psPath.empty())
    {
        psoDesc.psShader = GetAsset<ShaderAsset>(psPath.stem())->shader;
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
    
    asset->pso = std::make_shared<PipelineStateObject>();
    if (!GraphicsEngine::Get().CreatePSO(asset->pso, psoDesc))
    {
        LOG(LogAssetManager, Error, "Failed to create pso asset {}", asset->path.filename().string());
        return false;
    }

    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered PSO asset {}", asset->name.string());
    return true;
}

bool AssetManager::RegisterFontAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("json")) return false;

    std::shared_ptr<FontAsset> asset = std::make_shared<FontAsset>();
    asset->font = std::make_shared<Font>();

    std::filesystem::path texturePath = assetPath;
    texturePath = texturePath.replace_filename("T" + texturePath.stem().string().substr(1) + ".dds");
    if (myAssets.find(texturePath.stem()) == myAssets.end())
    {
        if (!RegisterTextureAsset(myContentRoot / texturePath))
        {
            LOG(LogAssetManager, Error, "Failed to create a font texture at path {}", asset->path.string());
            return false;
        }
    }

    asset->font->Texture = GetAsset<TextureAsset>(texturePath.stem())->texture;

    std::ifstream path(aPath);
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        LOG(LogAssetManager, Error, "Failed to read font asset {}, {}", asset->path.filename().string(), e.what());
        return false;
    }
    path.close();

    Font& font = *asset->font;

    font.Atlas.Size = data["atlas"]["size"];
    font.Atlas.Width = data["atlas"]["width"];
    font.Atlas.Height = data["atlas"]["height"];
    font.Atlas.EmSize = data["metrics"]["emSize"];
    font.Atlas.LineHeight = data["metrics"]["lineHeight"];
    font.Atlas.Ascender = data["metrics"]["ascender"];
    font.Atlas.Descender = data["metrics"]["descender"];

    size_t glyphCount = data["glyphs"].size();
    for (int i = 0; i < glyphCount; i++)
    {
        Font::Glyph newGlyph;
        nl::json glyph = data["glyphs"][i];
        unsigned unicode = glyph.value("unicode", 0);
        float advance = glyph.value("advance", -1.0f);

        CU::Vector4f planeBounds;
        if (glyph.contains("planeBounds"))
        {
            planeBounds.x = glyph["planeBounds"]["left"].get<float>();
            planeBounds.y = glyph["planeBounds"]["bottom"].get<float>();
            planeBounds.z = glyph["planeBounds"]["right"].get<float>();
            planeBounds.w = glyph["planeBounds"]["top"].get<float>();
        }

        CU::Vector4f uvBounds;
        float atlasWidth = static_cast<float>(font.Atlas.Width);
        float atlasHeight = static_cast<float>(font.Atlas.Height);
        if (glyph.contains("atlasBounds"))
        {
            float UVStartX = glyph["atlasBounds"]["left"].get<float>() / atlasWidth;
            float UVStartY = glyph["atlasBounds"]["bottom"].get<float>() / atlasHeight;
            float UVEndX = glyph["atlasBounds"]["right"].get<float>() / atlasWidth;
            float UVEndY = glyph["atlasBounds"]["top"].get<float>() / atlasHeight;

            uvBounds = { UVStartX, UVStartY, UVEndX, UVEndY };
        }

        newGlyph.Character = static_cast<char>(unicode);
        newGlyph.Advance = advance;
        newGlyph.PlaneBounds = planeBounds;
        newGlyph.UVBounds = uvBounds;
        font.Glyphs.emplace(unicode, newGlyph);
    }

    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered font asset {}", asset->path.filename().string());
    return true;
}

bool AssetManager::RegisterNavMeshAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::NavMesh tgaNavMesh;
    TGA::FBX::Importer::LoadNavMesh(aPath, tgaNavMesh, true);

    // Create Nav Polygons.
    std::vector<NavPolygon> navPolygons;
    for (auto& tgaChunk : tgaNavMesh.Chunks)
    {
        for (auto& tgaPolygon : tgaChunk.Polygons)
        {
            assert(tgaPolygon.Indices.size() < 4 && "Navmesh isn't triangulated >:((");
            NavPolygon& navPolygon = navPolygons.emplace_back(NavPolygon());
            for (auto& polygonIndex : tgaPolygon.Indices)
            {
                CU::Vector3f vertexPos = { tgaChunk.Vertices[polygonIndex].Position[0], tgaChunk.Vertices[polygonIndex].Position[1], tgaChunk.Vertices[polygonIndex].Position[2] };
                navPolygon.vertexPositions.emplace_back(vertexPos);
            }
        }
    }

    // Create Nav nodes with connections between eachother.
    std::unordered_map<unsigned, std::vector<unsigned>> connections;
    std::vector<NavNode> navNodes;

    for (int i = 0; i < static_cast<int>(navPolygons.size()); ++i)
    {
        NavPolygon& navPoly1 = navPolygons[i];

        for (int j = 0; j < static_cast<int>(navPolygons.size()); ++j)
        {
            if (i == j) break;

            NavPolygon& navPoly2 = navPolygons[j];

            int sharedVertices = 0;

            for (int vertexPos = 0; vertexPos < static_cast<int>(navPoly2.vertexPositions.size()); vertexPos++)
            {
                if (std::find(navPoly1.vertexPositions.begin(), navPoly1.vertexPositions.end(), navPoly2.vertexPositions[vertexPos]) != navPoly1.vertexPositions.end())
                {
                    sharedVertices++;
                }
            }

            if (sharedVertices == 2) // Polygons share an edge
            {
                connections[i].emplace_back(j);
                connections[j].emplace_back(i);
            }
        }

        NavNode& navNode = navNodes.emplace_back(NavNode());
        CU::Vector3f pos;
        int numVertices = static_cast<int>(navPoly1.vertexPositions.size());
        for (int vertexPos = 0; vertexPos < numVertices; vertexPos++)
        {
            pos += navPoly1.vertexPositions[vertexPos];
        }

        pos.x /= numVertices;
        pos.y /= numVertices;
        pos.z /= numVertices;

        navNode.position = pos;
    }

    for (int i = 0; i < static_cast<int>(navNodes.size()); ++i)
    {
        for (auto& nodeConnection : connections[i])
        {
            navNodes[i].edges.emplace_back(nodeConnection);
            navNodes[i].edgeCosts.emplace_back(static_cast<int>((navNodes[i].position - navNodes[nodeConnection].position).Length()));
        }
    }

    NavMesh navMesh;
    navMesh.Init(navNodes, navPolygons);
    CU::Vector3f boxExtents = { tgaNavMesh.BoxSphereBounds.BoxExtents[0], tgaNavMesh.BoxSphereBounds.BoxExtents[1], tgaNavMesh.BoxSphereBounds.BoxExtents[2] };
    CU::Vector3f boxCenter = { tgaNavMesh.BoxSphereBounds.Center[0], tgaNavMesh.BoxSphereBounds.Center[1], tgaNavMesh.BoxSphereBounds.Center[2] };
    navMesh.SetBoundingBox(boxCenter, boxExtents * 2.0f);

    std::shared_ptr<NavMeshAsset> asset = std::make_shared<NavMeshAsset>();
    asset->navmesh = std::make_shared<NavMesh>(std::move(navMesh));
    asset->path = assetPath;
    asset->name = assetPath.stem();
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered navmesh asset {}", asset->path.filename().string());
    return true;
}

AssetManager::AssetManager()
{
    TGA::FBX::Importer::InitImporter();
}

AssetManager::~AssetManager()
{
    TGA::FBX::Importer::UninitImporter();
}

bool AssetManager::ValidateAsset(const std::filesystem::path& aPath)
{
    if (!aPath.has_extension())
    {
        LOG(LogAssetManager, Error, "Path '{}' does not contain an extension!", aPath.string());
        return false;
    }

    if (!std::filesystem::exists(aPath))
    {
        LOG(LogAssetManager, Error, "Could not find asset at path '{}'!", aPath.string());
        return false;
    }

    std::filesystem::path assetPath = MakeRelative(aPath);
    if (myAssets.find(assetPath.stem()) != myAssets.end())
    {
        //LOG(LogAssetManager, Warning, "Asset with name '{}' is already registered!", aPath.string());
        return false;
    }

    return true;
}

std::filesystem::path AssetManager::MakeRelative(const std::filesystem::path& aPath)
{
    std::filesystem::path targetPath = relative(aPath, myContentRoot);
    targetPath = myContentRoot / targetPath;
    targetPath = weakly_canonical(targetPath);

    //auto [rootEnd, nothing] = std::mismatch(myContentRoot.begin(), myContentRoot.end(), targetPath.begin());
    //if (rootEnd != myContentRoot.end())
    //{
    //    LOG(AssetManagerLog, Error, "Path '{}' is not below the root '{}'!", targetPath.string(), myContentRoot.string());
    //    throw std::invalid_argument("Path is not below the root!");
    //}

    if (aPath.is_absolute())
    {
        return relative(aPath, myContentRoot);
    }

    return aPath;
}

bool AssetManager::DoesAssetExist(const std::filesystem::path& aPath)
{
    aPath;
    return false;
}

bool AssetManager::RegisterEngineTextureAsset(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize)
{
    std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>();
    asset->texture = std::make_shared<Texture>();
    if (!GraphicsEngine::Get().LoadTexture(aName, aTextureDataPtr, aTextureDataSize, *asset->texture))
    {
        LOG(LogAssetManager, Error, "Failed to register default texture asset {}", aName);
        return false;
    }
    asset->name = aName;
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered default texture asset {}", aName);

    return true;
}

bool AssetManager::RegisterPlanePrimitive()
{
    float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    float uv[4][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}}
    };
    float pos[4][4] = {
        {1, 0, 1, 1},
        {1, 0, -1, 1},
        {-1, 0, -1, 1},
        {-1, 0, 1, 1}
    };
    float normals[4][3] = {
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 0 }
    };
    float tangents[4][3] = {
        { 1, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 0 }
    };

    std::vector<Vertex> vertexList;
    vertexList.emplace_back(Vertex(pos[0], emptyColor, uv[0], normals[0], tangents[0]));
    vertexList.emplace_back(Vertex(pos[1], emptyColor, uv[1], normals[1], tangents[1]));
    vertexList.emplace_back(Vertex(pos[2], emptyColor, uv[2], normals[2], tangents[2]));
    vertexList.emplace_back(Vertex(pos[3], emptyColor, uv[3], normals[3], tangents[3]));

    std::vector<unsigned> indexList = {
        0, 1, 2,
        2, 3, 0,
    };

    Mesh::Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());
    std::vector<Mesh::Element> elementList = { element };

    Mesh plane;
    plane.InitBoundingBox({ -1.0f, -0.001f, -1.0f }, { 1.0f, 0.001f, 1.0f });
    plane.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());
    
    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(plane));
    asset->name = "SM_PlanePrimitive";
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}

bool AssetManager::RegisterCubePrimitive()
{
    float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    float uv[24][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                          {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                          {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
    };

    float pos[24][4] = {
        {1,1,-1, 1}, // BACK
        {1,-1,-1, 1},
        {-1,-1,-1, 1},
        {-1,1,-1, 1},

        {1,1,1, 1}, // FRONT
        {-1,1,1, 1},
        {-1,-1,1, 1},
        {1,-1,1, 1},

        {1,1,-1, 1}, // RIGHT
        {1,1,1, 1},
        {1,-1,1, 1},
        {1,-1,-1, 1},

        {1,-1,-1, 1}, // DOWN
        {1,-1,1, 1},
        {-1,-1,1, 1},
        {-1,-1,-1, 1},

        {-1,-1,-1, 1}, // LEFT
        {-1,-1,1, 1},
        {-1,1,1, 1},
        {-1,1,-1, 1},

        {1,1,1, 1}, // UP
        {1,1,-1, 1},
        {-1,1,-1, 1},
        {-1,1,1, 1},
    };

    float normals[24][3] = {
    { 0, 0, -1 }, // BACK
    { 0, 0, -1 },
    { 0, 0, -1 },
    { 0, 0, -1 },

    { 0, 0, 1 }, // FRONT
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },

    { 1, 0, 0 }, // RIGHT
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },

    { 0, -1, 0 }, // DOWN
    { 0, -1, 0 },
    { 0, -1, 0 },
    { 0, -1, 0 },

    { -1, 0, 0 }, // LEFT
    { -1, 0, 0 },
    { -1, 0, 0 },
    { -1, 0, 0 },

    { 0, 1, 0 }, // UP
    { 0, 1, 0 },
    { 0, 1, 0 },
    { 0, 1, 0 }
    };
    float tangents[24][3] = {
    { 1, 0, 0 }, // BACK
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },

    { -1, 0, 0 }, // FRONT
    { -1, 0, 0 },
    { -1, 0, 0 },
    { -1, 0, 0 },

    { 0, 0, 1 }, // RIGHT
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },

    { 1, 0, 0 }, // DOWN
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },

    { 0, 0, -1 }, // LEFT
    { 0, 0, -1 },
    { 0, 0, -1 },
    { 0, 0, -1 },

    { 1, 0, 0 }, // UP
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 }
    };

    std::vector<Vertex> vertexList;

    vertexList.emplace_back(Vertex(pos[0], emptyColor, uv[0], normals[0], tangents[0]));
    vertexList.emplace_back(Vertex(pos[1], emptyColor, uv[1], normals[1], tangents[1]));
    vertexList.emplace_back(Vertex(pos[2], emptyColor, uv[2], normals[2], tangents[2]));
    vertexList.emplace_back(Vertex(pos[3], emptyColor, uv[3], normals[3], tangents[3]));
    vertexList.emplace_back(Vertex(pos[4], emptyColor, uv[4], normals[4], tangents[4]));
    vertexList.emplace_back(Vertex(pos[5], emptyColor, uv[5], normals[5], tangents[5]));
    vertexList.emplace_back(Vertex(pos[6], emptyColor, uv[6], normals[6], tangents[6]));
    vertexList.emplace_back(Vertex(pos[7], emptyColor, uv[7], normals[7], tangents[7]));
    vertexList.emplace_back(Vertex(pos[8], emptyColor, uv[8], normals[8], tangents[8]));
    vertexList.emplace_back(Vertex(pos[9], emptyColor, uv[9], normals[9], tangents[9]));
    vertexList.emplace_back(Vertex(pos[10], emptyColor, uv[10], normals[10], tangents[10]));
    vertexList.emplace_back(Vertex(pos[11], emptyColor, uv[11], normals[11], tangents[11]));
    vertexList.emplace_back(Vertex(pos[12], emptyColor, uv[12], normals[12], tangents[12]));
    vertexList.emplace_back(Vertex(pos[13], emptyColor, uv[13], normals[13], tangents[13]));
    vertexList.emplace_back(Vertex(pos[14], emptyColor, uv[14], normals[14], tangents[14]));
    vertexList.emplace_back(Vertex(pos[15], emptyColor, uv[15], normals[15], tangents[15]));
    vertexList.emplace_back(Vertex(pos[16], emptyColor, uv[16], normals[16], tangents[16]));
    vertexList.emplace_back(Vertex(pos[17], emptyColor, uv[17], normals[17], tangents[17]));
    vertexList.emplace_back(Vertex(pos[18], emptyColor, uv[18], normals[18], tangents[18]));
    vertexList.emplace_back(Vertex(pos[19], emptyColor, uv[19], normals[19], tangents[19]));
    vertexList.emplace_back(Vertex(pos[20], emptyColor, uv[20], normals[20], tangents[20]));
    vertexList.emplace_back(Vertex(pos[21], emptyColor, uv[21], normals[21], tangents[21]));
    vertexList.emplace_back(Vertex(pos[22], emptyColor, uv[22], normals[22], tangents[22]));
    vertexList.emplace_back(Vertex(pos[23], emptyColor, uv[23], normals[23], tangents[23]));

    std::vector<unsigned> indexList = {
        0,1,2,
        0,2,3,
        4,5,6,
        4,6,7,
        8,9,10,
        8,10,11,
        12,13,14,
        12,14,15,
        16,17,18,
        16,18,19,
        20,21,22,
        20,22,23
    };

    Mesh::Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());
    std::vector<Mesh::Element> elementList = { element };

    Mesh cube;
    cube.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
    cube.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(cube));
    asset->name = "SM_CubePrimitive";
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}

bool AssetManager::RegisterRampPrimitive()
{
    float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
    float uv[18][4][2] = { {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

                           {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
                           {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
    };

    float pos[18][4] = {
        {-1, 1, 1, 1}, // LEFT
        {-1, 1, -1, 1},
        {-1, -1, -1, 1},
        {-1, -1, 1, 1},

        {1, -1, -1, 1}, // DOWN
        {1, -1, 1, 1},
        {-1, -1, 1, 1},
        {-1, -1, -1, 1},

        {-1, 1, 1, 1}, // DIAGONAL
        {1, -1, 1, 1},
        {1, -1, -1, 1},
        {-1, 1, -1, 1},

        {-1, 1, 1, 1}, // FRONT
        {-1, -1, 1, 1},
        {1, -1, 1, 1},

        {1, -1, -1, 1}, // BACK
        {-1, -1, -1, 1},
        { -1, 1, -1, 1 },
    };

    float normals[18][3] = {
        { -1, 0, 0 }, // LEFT
        { -1, 0, 0 },
        { -1, 0, 0 },
        { -1, 0, 0 },

        { 0, -1, 0 }, // DOWN
        { 0, -1, 0 },
        { 0, -1, 0 },
        { 0, -1, 0 },

        { 1, 1, 0 }, // DIAGONAL
        { 1, 1, 0 },
        { 1, 1, 0 },
        { 1, 1, 0 },

        { 0, 0, 1 }, // FRONT
        { 0, 0, 1 },
        { 0, 0, 1 },

        { 0, 0, -1 }, // BACK
        { 0, 0, -1 },
        { 0, 0, -1 }
    };

    float tangents[18][3] = {
        { 0, 0, -1 }, // LEFT
        { 0, 0, -1 },
        { 0, 0, -1 },
        { 0, 0, -1 },

        { 0, 0, 1 }, // DOWN
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 },

        { 0, 0, 1 }, // DIAGONAL
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 },

        { -1, 0, 0 }, // FRONT
        { -1, 0, 0 },
        { -1, 0, 0 },

        { 1, 0, 0 }, // BACK
        { 1, 0, 0 },
        { 1, 0, 0 }
    };

    std::vector<Vertex> vertexList;
    vertexList.emplace_back(Vertex(pos[0], emptyColor, uv[0], normals[0], tangents[0]));
    vertexList.emplace_back(Vertex(pos[1], emptyColor, uv[1], normals[1], tangents[1]));
    vertexList.emplace_back(Vertex(pos[2], emptyColor, uv[2], normals[2], tangents[2]));
    vertexList.emplace_back(Vertex(pos[3], emptyColor, uv[3], normals[3], tangents[3]));
    vertexList.emplace_back(Vertex(pos[4], emptyColor, uv[4], normals[4], tangents[4]));
    vertexList.emplace_back(Vertex(pos[5], emptyColor, uv[5], normals[5], tangents[5]));
    vertexList.emplace_back(Vertex(pos[6], emptyColor, uv[6], normals[6], tangents[6]));
    vertexList.emplace_back(Vertex(pos[7], emptyColor, uv[7], normals[7], tangents[7]));
    vertexList.emplace_back(Vertex(pos[8], emptyColor, uv[8], normals[8], tangents[8]));
    vertexList.emplace_back(Vertex(pos[9], emptyColor, uv[9], normals[9], tangents[9]));
    vertexList.emplace_back(Vertex(pos[10], emptyColor, uv[10], normals[10], tangents[10]));
    vertexList.emplace_back(Vertex(pos[11], emptyColor, uv[11], normals[11], tangents[11]));
    vertexList.emplace_back(Vertex(pos[12], emptyColor, uv[12], normals[12], tangents[12]));
    vertexList.emplace_back(Vertex(pos[13], emptyColor, uv[13], normals[13], tangents[13]));
    vertexList.emplace_back(Vertex(pos[14], emptyColor, uv[14], normals[14], tangents[14]));
    vertexList.emplace_back(Vertex(pos[15], emptyColor, uv[15], normals[15], tangents[15]));
    vertexList.emplace_back(Vertex(pos[16], emptyColor, uv[16], normals[16], tangents[16]));
    vertexList.emplace_back(Vertex(pos[17], emptyColor, uv[17], normals[17], tangents[17]));

    std::vector<unsigned> indexList = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        15, 16, 17,
    };

    Mesh::Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());
    std::vector<Mesh::Element> elementList = { element };

    Mesh ramp;
    ramp.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
    ramp.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(ramp));
    asset->name = "SM_RampPrimitive";
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}

void AssetManager::LogAssetLoadError(const std::filesystem::path& aPath)
{
    LOG(LogAssetManager, Error, "Asset manager can not find asset at path: {}", aPath.string());
    MessageBox(NULL, L"Asset manager can not find asset, Please check the log for more information!", L"Asset Manager Error", MB_ICONERROR);
}
