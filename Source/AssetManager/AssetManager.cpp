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
#include "CommonUtilities/StringUtilities.hpp"


#include "DefaultTextures/Default_C.h"
#include "DefaultTextures/Default_N.h"
#include "DefaultTextures/Default_M.h"
#include "DefaultTextures/Default_FX.h"

AssetManager::AssetManager()
{
    TGA::FBX::Importer::InitImporter();
}

AssetManager::~AssetManager()
{
    TGA::FBX::Importer::UninitImporter();
}

bool AssetManager::DeregisterAsset(const std::filesystem::path& aPath)
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

bool AssetManager::DeregisterAsset(const std::shared_ptr<Asset> aAsset)
{
    return DeregisterAsset(aAsset->path);
}

bool AssetManager::RegisterAsset(const std::filesystem::path& aPath)
{
    std::string name = aPath.filename().string();
    Utilities::ToLower(name);
    if (name.starts_with("sm") || name.starts_with("sk"))
    {
        return RegisterMeshAsset(aPath);
    }
    else if (name.starts_with("a"))
    {
        return RegisterAnimationAsset(aPath);
    }
    else if (name.starts_with("mat"))
    {
        return RegisterMaterialAsset(aPath);
    }
    else if (name.starts_with("t"))
    {
        return RegisterTextureAsset(aPath);
    }
    else if (name.starts_with("sh"))
    {
        return RegisterShaderAsset(aPath);
    }
    else if (name.starts_with("pso"))
    {
        return RegisterPSOAsset(aPath);
    }
    else if (name.starts_with("f"))
    {
        return RegisterFontAsset(aPath);
    }
    else if (name.starts_with("nm"))
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
    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot))
    {
        if (file.path().has_filename() && file.path().has_extension())
        {
            myFutures.emplace_back(std::async(std::launch::async, &AssetManager::RegisterAsset, this, file.path()));
        }
    }

    // Game currently has no way of accessing an asset that hasn't finished loading in. This can be removed when that is implemented.
    for (auto& future : myFutures)
    {
        future.wait();
    }

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered all assets in {}ms", std::round(loadTime.count()));
}

bool AssetManager::RegisterMeshAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::Mesh tgaMesh;
    {
        std::lock_guard<std::mutex> importerLock(myImporterMutex);
        TGA::FBX::Importer::LoadMesh(aPath, tgaMesh);
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Mesh::Element> elements(tgaMesh.Elements.size());

    Math::Vector3f minBBPoint;
    Math::Vector3f maxBBPoint;

    unsigned nextVertexOffset = 0;
    unsigned nextIndexOffset = 0;

    for (auto& tgaElement : tgaMesh.Elements)
    {
        Mesh::Element& element = elements.emplace_back();
        element.VertexOffset = nextVertexOffset;
        element.IndexOffset = nextIndexOffset;
        element.NumVertices = static_cast<int>(tgaElement.Vertices.size());
        element.NumIndices = static_cast<int>(tgaElement.Indices.size());
        element.MaterialIndex = tgaElement.MaterialIndex;

        vertices.reserve(vertices.size() + tgaElement.Vertices.size());
        for (auto& v : tgaElement.Vertices)
        {
            vertices.emplace_back(v.Position, v.VertexColors, v.BoneIDs, v.BoneWeights, v.UVs, v.Normal, v.Tangent);
            nextVertexOffset++;

            minBBPoint.x = v.Position[0] < minBBPoint.x ? v.Position[0] : minBBPoint.x;
            minBBPoint.y = v.Position[1] < minBBPoint.y ? v.Position[1] : minBBPoint.y;
            minBBPoint.z = v.Position[2] < minBBPoint.z ? v.Position[2] : minBBPoint.z;

            maxBBPoint.x = v.Position[0] > maxBBPoint.x ? v.Position[0] : maxBBPoint.x;
            maxBBPoint.y = v.Position[1] > maxBBPoint.y ? v.Position[1] : maxBBPoint.y;
            maxBBPoint.z = v.Position[2] > maxBBPoint.z ? v.Position[2] : maxBBPoint.z;
        }

        indices.reserve(indices.size() + tgaElement.Indices.size());
        for (auto& i : tgaElement.Indices)
        {
            indices.emplace_back(i + element.VertexOffset);
            nextIndexOffset++;
        }
    }

    Mesh::Skeleton skeleton;
    skeleton.myJoints.reserve(tgaMesh.Skeleton.Bones.size());

    for (auto& tgaJoint : tgaMesh.Skeleton.Bones)
    {
        Mesh::Skeleton::Joint& joint = skeleton.myJoints.emplace_back();
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

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered mesh asset {} in {}ms", asset->path.filename().string(), std::round(loadTime.count()));
    return true;
}

bool AssetManager::RegisterAnimationAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::Animation tgaAnimation;

    {
        std::lock_guard<std::mutex> importerLock(myImporterMutex);
        TGA::FBX::Importer::LoadAnimation(aPath, tgaAnimation);
    }

    Animation animation;
    animation.Duration = static_cast<float>(tgaAnimation.Duration);
    animation.FramesPerSecond = tgaAnimation.FramesPerSecond;

    for (auto& tgaAnimFrame : tgaAnimation.Frames)
    {
        Animation::Frame frame;
        for (auto& tgaAnimFrameJoint : tgaAnimFrame.LocalTransforms)
        {
            auto& matrix = tgaAnimFrameJoint.second.Data;
            Math::Matrix4x4<float> jointTransform = { matrix[0], matrix[1], matrix[2], matrix[3],
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

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered animation asset {} in {}ms", asset->path.filename().string(), std::round(loadTime.count()));
    return true;
}

bool AssetManager::RegisterMaterialAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

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
                RegisterTextureAsset(myContentRoot / texPath);
                textureExists = myAssets.find(texName) != myAssets.end();
            }

            if (textureExists)
            {
                asset->material->SetTextureOnSlot(textureIndex, GetAsset<TextureAsset>(texName)->texture);
            }
            else
            {
                LOG(LogAssetManager, Error, "Texture Asset {} could not be found!", texPath.string());
            }

            textureIndex++;
        }
    }

    asset->path = assetPath;
    asset->name = assetPath.stem();

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered material asset {} in {}ms", asset->path.filename().string(), std::round(loadTime.count()));
    return true;
}

bool AssetManager::RegisterTextureAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

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

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered texture asset {} in {}ms", asset->path.filename().string(), std::round(loadTime.count()));
    return true;
}

bool AssetManager::RegisterShaderAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

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

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered shader asset {} in {}ms", asset->path.filename().string(), std::round(loadTime.count()));
    return true;
}

bool AssetManager::RegisterPSOAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

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

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered PSO asset {} in {}ms", asset->name.string(), std::round(loadTime.count()));
    return true;
}

bool AssetManager::RegisterFontAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

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

        Math::Vector4f planeBounds;
        if (glyph.contains("planeBounds"))
        {
            planeBounds.x = glyph["planeBounds"]["left"].get<float>();
            planeBounds.y = glyph["planeBounds"]["bottom"].get<float>();
            planeBounds.z = glyph["planeBounds"]["right"].get<float>();
            planeBounds.w = glyph["planeBounds"]["top"].get<float>();
        }

        Math::Vector4f uvBounds;
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

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered font asset {} in {}ms", asset->path.filename().string(), std::round(loadTime.count()));
    return true;
}

std::vector<NavPolygon> CreateNavPolygons(const TGA::FBX::NavMesh& tgaNavMesh);
std::vector<NavNode> CreateNavNodes(const std::vector<NavPolygon>& navPolygons);
std::vector<NavPortal> CreateNavPortals(const std::vector<NavPolygon>& navPolygons, const std::vector<NavNode>& aNavNodes);

bool AssetManager::RegisterNavMeshAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

    std::filesystem::path assetPath = MakeRelative(aPath);
    const std::string ext = assetPath.extension().string();
    if (!ext.ends_with("fbx")) return false;

    TGA::FBX::NavMesh tgaNavMesh;
    {
        std::lock_guard<std::mutex> importerLock(myImporterMutex);
        TGA::FBX::Importer::LoadNavMesh(aPath, tgaNavMesh, true);
    }

    // Create Nav Polygons.
    std::vector<NavPolygon> navPolygons = CreateNavPolygons(tgaNavMesh);

    // Create Nav nodes with connections between eachother.
    std::vector<NavNode> navNodes = CreateNavNodes(navPolygons);

    // Create Nav Portals.
    std::vector<NavPortal> navPortals = CreateNavPortals(navPolygons, navNodes);

    for (int portalIndex = 0; portalIndex < static_cast<int>(navPortals.size()); ++portalIndex)
    {
        NavPortal& portal = navPortals[portalIndex];
        navNodes[portal.nodes[0]].portals.emplace_back(portalIndex);
    }

    NavMesh navMesh;
    navMesh.Init(navNodes, navPolygons, navPortals);
    Math::Vector3f boxExtents = { tgaNavMesh.BoxSphereBounds.BoxExtents[0], tgaNavMesh.BoxSphereBounds.BoxExtents[1], tgaNavMesh.BoxSphereBounds.BoxExtents[2] };
    Math::Vector3f boxCenter = { tgaNavMesh.BoxSphereBounds.Center[0], tgaNavMesh.BoxSphereBounds.Center[1], tgaNavMesh.BoxSphereBounds.Center[2] };
    navMesh.SetBoundingBox(boxCenter, boxExtents * 2.0f);

    std::shared_ptr<NavMeshAsset> asset = std::make_shared<NavMeshAsset>();
    asset->navmesh = std::make_shared<NavMesh>(std::move(navMesh));
    asset->path = assetPath;
    asset->name = assetPath.stem();

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
    LOG(LogAssetManager, Log, "Registered navmesh asset {} in {}ms", asset->path.filename().string(), std::round(loadTime.count()));
    return true;
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
        //LOG(LogAssetManager, Warning, "Asset with name '{}' is already registered!", assetPath.stem().string());
        return false;
    }

    return true;
}

std::filesystem::path AssetManager::MakeRelative(const std::filesystem::path& aPath) const
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

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
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
    vertexList.reserve(4);
    vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
    vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
    vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
    vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);

    std::vector<unsigned> indexList = {
        0, 1, 2,
        2, 3, 0,
    };

    std::vector<Mesh::Element> elementList;
    Mesh::Element& element = elementList.emplace_back();
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());

    Mesh plane;
    plane.InitBoundingBox({ -1.0f, -0.001f, -1.0f }, { 1.0f, 0.001f, 1.0f });
    plane.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());
    
    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(plane));
    asset->name = "SM_PlanePrimitive";

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
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
    vertexList.reserve(24);

    vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
    vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
    vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
    vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);
    vertexList.emplace_back(pos[4], emptyColor, uv[4], normals[4], tangents[4]);
    vertexList.emplace_back(pos[5], emptyColor, uv[5], normals[5], tangents[5]);
    vertexList.emplace_back(pos[6], emptyColor, uv[6], normals[6], tangents[6]);
    vertexList.emplace_back(pos[7], emptyColor, uv[7], normals[7], tangents[7]);
    vertexList.emplace_back(pos[8], emptyColor, uv[8], normals[8], tangents[8]);
    vertexList.emplace_back(pos[9], emptyColor, uv[9], normals[9], tangents[9]);
    vertexList.emplace_back(pos[10], emptyColor, uv[10], normals[10], tangents[10]);
    vertexList.emplace_back(pos[11], emptyColor, uv[11], normals[11], tangents[11]);
    vertexList.emplace_back(pos[12], emptyColor, uv[12], normals[12], tangents[12]);
    vertexList.emplace_back(pos[13], emptyColor, uv[13], normals[13], tangents[13]);
    vertexList.emplace_back(pos[14], emptyColor, uv[14], normals[14], tangents[14]);
    vertexList.emplace_back(pos[15], emptyColor, uv[15], normals[15], tangents[15]);
    vertexList.emplace_back(pos[16], emptyColor, uv[16], normals[16], tangents[16]);
    vertexList.emplace_back(pos[17], emptyColor, uv[17], normals[17], tangents[17]);
    vertexList.emplace_back(pos[18], emptyColor, uv[18], normals[18], tangents[18]);
    vertexList.emplace_back(pos[19], emptyColor, uv[19], normals[19], tangents[19]);
    vertexList.emplace_back(pos[20], emptyColor, uv[20], normals[20], tangents[20]);
    vertexList.emplace_back(pos[21], emptyColor, uv[21], normals[21], tangents[21]);
    vertexList.emplace_back(pos[22], emptyColor, uv[22], normals[22], tangents[22]);
    vertexList.emplace_back(pos[23], emptyColor, uv[23], normals[23], tangents[23]);

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

    std::vector<Mesh::Element> elementList;
    Mesh::Element& element = elementList.emplace_back();
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());

    Mesh cube;
    cube.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
    cube.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(cube));
    asset->name = "SM_CubePrimitive";

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
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
    vertexList.reserve(18);

    vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
    vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
    vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
    vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);
    vertexList.emplace_back(pos[4], emptyColor, uv[4], normals[4], tangents[4]);
    vertexList.emplace_back(pos[5], emptyColor, uv[5], normals[5], tangents[5]);
    vertexList.emplace_back(pos[6], emptyColor, uv[6], normals[6], tangents[6]);
    vertexList.emplace_back(pos[7], emptyColor, uv[7], normals[7], tangents[7]);
    vertexList.emplace_back(pos[8], emptyColor, uv[8], normals[8], tangents[8]);
    vertexList.emplace_back(pos[9], emptyColor, uv[9], normals[9], tangents[9]);
    vertexList.emplace_back(pos[10], emptyColor, uv[10], normals[10], tangents[10]);
    vertexList.emplace_back(pos[11], emptyColor, uv[11], normals[11], tangents[11]);
    vertexList.emplace_back(pos[12], emptyColor, uv[12], normals[12], tangents[12]);
    vertexList.emplace_back(pos[13], emptyColor, uv[13], normals[13], tangents[13]);
    vertexList.emplace_back(pos[14], emptyColor, uv[14], normals[14], tangents[14]);
    vertexList.emplace_back(pos[15], emptyColor, uv[15], normals[15], tangents[15]);
    vertexList.emplace_back(pos[16], emptyColor, uv[16], normals[16], tangents[16]);
    vertexList.emplace_back(pos[17], emptyColor, uv[17], normals[17], tangents[17]);

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

    std::vector<Mesh::Element> elementList;
    Mesh::Element& element = elementList.emplace_back();
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertexList.size());
    element.NumIndices = static_cast<unsigned>(indexList.size());

    Mesh ramp;
    ramp.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
    ramp.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());

    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->mesh = std::make_shared<Mesh>(std::move(ramp));
    asset->name = "SM_RampPrimitive";

    std::lock_guard<std::mutex> assetLock(myAssetMutex);
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}

void AssetManager::LogAssetLoadError(const std::filesystem::path& aPath)
{
    LOG(LogAssetManager, Error, "Asset manager can not find asset at path: {}", aPath.string());
    MessageBox(NULL, L"Asset manager can not find asset, Please check the log for more information!", L"Asset Manager Error", MB_ICONERROR);
}

std::vector<NavPolygon> CreateNavPolygons(const TGA::FBX::NavMesh& tgaNavMesh)
{
    std::vector<NavPolygon> navPolygons;
    for (auto& tgaChunk : tgaNavMesh.Chunks)
    {
        navPolygons.reserve(navPolygons.size() + tgaChunk.Polygons.size());

        for (auto& tgaPolygon : tgaChunk.Polygons)
        {
            assert(tgaPolygon.Indices.size() < 4 && "Navmesh isn't triangulated >:((");
            NavPolygon& navPolygon = navPolygons.emplace_back();
            for (std::size_t i = 0; i < tgaPolygon.Indices.size(); ++i)
            {
                const int vertexIndex = tgaPolygon.Indices[i];

                Math::Vector3f vertexPos = { tgaChunk.Vertices[vertexIndex].Position[0], tgaChunk.Vertices[vertexIndex].Position[1], tgaChunk.Vertices[vertexIndex].Position[2] };
                navPolygon.vertexPositions[i] = vertexPos;
            }
        }
    }

    return navPolygons;
}

std::vector<NavNode> CreateNavNodes(const std::vector<NavPolygon>& navPolygons)
{
    std::vector<NavNode> navNodes(navPolygons.size());

    for (int i = 0; i < static_cast<int>(navPolygons.size()); ++i)
    {
        NavNode& navNode = navNodes.emplace_back();
        Math::Vector3f pos;
        int numVertices = static_cast<int>(navPolygons[i].vertexPositions.size());
        for (int vertexPos = 0; vertexPos < numVertices; vertexPos++)
        {
            pos += navPolygons[i].vertexPositions[vertexPos];
        }

        pos.x /= numVertices;
        pos.y /= numVertices;
        pos.z /= numVertices;

        navNode.position = pos;
    }

    return navNodes;
}

std::vector<NavPortal> CreateNavPortals(const std::vector<NavPolygon>& navPolygons, const std::vector<NavNode>& aNavNodes)
{
    std::vector<NavPortal> navPortals;

    for (int i = 0; i < static_cast<int>(navPolygons.size()); ++i)
    {
        const NavPolygon& navPoly1 = navPolygons[i];

        for (int j = 0; j < static_cast<int>(navPolygons.size()); ++j)
        {
            if (i == j) continue;

            const NavPolygon& navPoly2 = navPolygons[j];

            std::array<Math::Vector3f, 2> sharedVertices;
            int sharedCount = 0;

            for (int vertexPos = 0; vertexPos < static_cast<int>(navPoly2.vertexPositions.size()); vertexPos++)
            {
                const Math::Vector3f& vertexPosToTest = navPoly2.vertexPositions[vertexPos];
                if (std::find(navPoly1.vertexPositions.begin(), navPoly1.vertexPositions.end(), vertexPosToTest) != navPoly1.vertexPositions.end())
                {
                    assert(sharedCount < 2 && "More than 2 shared vertices means mesh is not triangulated, or polygon is really damn small");
                    sharedVertices[sharedCount++] = vertexPosToTest;
                }
            }

            if (sharedCount == 2) // Polygons share an edge
            {
                NavPortal& navPortal = navPortals.emplace_back();
                navPortal.nodes[0] = i;
                navPortal.nodes[1] = j;
                navPortal.vertices[0] = sharedVertices[0];
                navPortal.vertices[1] = sharedVertices[1];
                navPortal.cost = (aNavNodes[i].position - aNavNodes[j].position).Length();
            }
        }
    }

    return navPortals;
}