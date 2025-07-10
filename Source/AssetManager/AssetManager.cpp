#include "Assetpch.h"

#include "AssetManager.h"
#include "Asset.h"
#include "GraphicsEngine.h"
#include "Objects/Vertices/Vertex.h"

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

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

bool AssetManager::RegisterAsset(const std::filesystem::path& aPath)
{
    if (!ValidateAsset(aPath)) return false;

    std::string extension = Utilities::ToLowerCopy(aPath.extension().string());
    std::string name = Utilities::ToLowerCopy(aPath.filename().string());

    if (extension == ".fbx")
    {
        if (name.starts_with("sm") || name.starts_with("sk"))
        {
            return RegisterMeshAsset(aPath);
        }
        else if (name.starts_with("a"))
        {
            return RegisterAnimationAsset(aPath);
        }
        else if (name.starts_with("nm"))
        {
            return RegisterNavMeshAsset(aPath);
        }
    }
    else if (extension == ".mat")
    {
        return RegisterMaterialAsset(aPath);
    }
    else if (extension == ".dds")
    {
        return RegisterTextureAsset(aPath);
    }
    else if (extension == ".cso")
    {
        return RegisterShaderAsset(aPath);
    }
    else if (extension == ".pso")
    {
        return RegisterPSOAsset(aPath);
    }
    else if (extension == ".font")
    {
        return RegisterFontAsset(aPath);
    }
    else if (extension == ".font")
    {
        return RegisterFontAsset(aPath);
    }

    LOG(LogAssetManager, Error, "Asset {} with extension {} could not be correctly identified!", name, extension);
    return false;
}

bool AssetManager::DeregisterAsset(const std::filesystem::path& aPath)
{
    if (!IsAssetRegistered(aPath))
    {
        LOG(LogAssetManager, Warning, "Couldn't unregister asset at path {} since it does not seem to exist!", aPath.string());
        return false;
    }

    std::filesystem::path filename = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.erase(filename);
    LOG(LogAssetManager, Log, "Successfully unregistered asset at path {}!", filename.string());
    return true;
}

bool AssetManager::IsAssetRegistered(const std::filesystem::path& aPath)
{
    std::filesystem::path filename = Utilities::ToLowerCopy(aPath.filename().string());
    return myAssets.find(filename) != myAssets.end();
}

bool AssetManager::LoadAsset(const std::filesystem::path& aPath)
{
    if (auto assetBase = GetAssetBase(aPath))
    {
        if (assetBase->Load())
        {
            assetBase->myIsLoaded = true;
            return true;
        }
    }

    return false;
}

bool AssetManager::UnloadAsset(const std::filesystem::path& aPath)
{
    if (auto assetBase = GetAssetBase(aPath))
    {
        if (assetBase->Unload())
        {
            assetBase->myIsLoaded = false;
            return true;
        }
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
    RegisterEngineTextureAsset("default_c", BuiltIn_Default_C_ByteCode, sizeof(BuiltIn_Default_C_ByteCode));
    RegisterEngineTextureAsset("default_n", BuiltIn_Default_N_ByteCode, sizeof(BuiltIn_Default_N_ByteCode));
    RegisterEngineTextureAsset("default_m", BuiltIn_Default_M_ByteCode, sizeof(BuiltIn_Default_M_ByteCode));
    RegisterEngineTextureAsset("default_fx", BuiltIn_Default_FX_ByteCode, sizeof(BuiltIn_Default_FX_ByteCode));

    std::shared_ptr<MaterialAsset> asset = std::make_shared<MaterialAsset>();
    asset->material = std::make_shared<Material>();
    asset->material->SetPSO(GraphicsEngine::Get().GetDefaultPSO());
    asset->material->MaterialSettings().albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
    asset->material->MaterialSettings().emissiveStrength = 0.0f;
    asset->material->SetTexture(Material::TextureType::Albedo, GetAsset<TextureAsset>("default_c")->texture);
    asset->material->SetTexture(Material::TextureType::Normal, GetAsset<TextureAsset>("default_n")->texture);
    asset->material->SetTexture(Material::TextureType::Material, GetAsset<TextureAsset>("default_m")->texture);
    asset->material->SetTexture(Material::TextureType::Effects, GetAsset<TextureAsset>("default_fx")->texture);
    asset->name = "defaultmaterial";
    asset->myIsLoaded = true;
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered material asset {}", asset->name.string());

    RegisterPlanePrimitive();
    RegisterCubePrimitive();
    RegisterRampPrimitive();

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Models/"))
    {
        if (!ValidateAsset(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".fbx")) continue;
        RegisterMeshAsset(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Textures/"))
    {
        if (!ValidateAsset(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".dds")) continue;
        RegisterTextureAsset(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Shaders/"))
    {
        if (!ValidateAsset(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".cso")) continue;
        RegisterShaderAsset(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/PSOs/"))
    {
        if (!ValidateAsset(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".pso")) continue;
        RegisterPSOAsset(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Materials/"))
    {
        if (!ValidateAsset(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".mat")) continue;
        RegisterMaterialAsset(file.path());
    }
}

void AssetManager::RegisterAllAssetsInDirectory()
{
    std::chrono::system_clock::time_point registerAllStartTime = std::chrono::system_clock::now();

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot))
    {
        if (!ValidateAsset(file.path())) continue;

        std::chrono::system_clock::time_point registerStartTime = std::chrono::system_clock::now();

        std::filesystem::path filename(Utilities::ToLowerCopy(file.path().filename().string()));
        if (RegisterAsset(file.path()))
        {
            std::chrono::duration<float, std::ratio<1, 1000>> registerTime = std::chrono::system_clock::now() - registerStartTime;
            LOG(LogAssetManager, Log, "Registered asset {} in {}ms", filename.string(), std::round(registerTime.count()));
        }
        else
        {
            LOG(LogAssetManager, Error, "Failed to register asset {}", filename.string());
        }
    }

    std::chrono::duration<float, std::ratio<1, 1000>> registerAllTime = std::chrono::system_clock::now() - registerAllStartTime;
    LOG(LogAssetManager, Log, "Registered all assets in {}ms", std::round(registerAllTime.count()));
}

void AssetManager::LoadAllRegisteredAssets()
{
    std::chrono::system_clock::time_point loadAllStartTime = std::chrono::system_clock::now();

    for (auto& [name, asset] : myAssets)
    {
        if (!name.has_extension()) continue;

        std::chrono::system_clock::time_point loadStartTime = std::chrono::system_clock::now();

        if (asset->Load())
        {
            asset->myIsLoaded = true;
            std::chrono::duration<float, std::ratio<1, 1000>> loadTime = std::chrono::system_clock::now() - loadStartTime;
            LOG(LogAssetManager, Log, "Loaded asset {} in {}ms", name.string(), std::round(loadTime.count()));
        }
        else
        {
            LOG(LogAssetManager, Error, "Failed to load asset {}", name.string());
        }
    }

    std::chrono::duration<float, std::ratio<1, 1000>> loadAllTime = std::chrono::system_clock::now() - loadAllStartTime;
    LOG(LogAssetManager, Log, "Loaded all assets in {}ms", std::round(loadAllTime.count()));
}

bool AssetManager::RegisterMeshAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());

    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::RegisterAnimationAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<AnimationAsset> asset = std::make_shared<AnimationAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::RegisterMaterialAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<MaterialAsset> asset = std::make_shared<MaterialAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::RegisterTextureAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::RegisterShaderAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<ShaderAsset> asset = std::make_shared<ShaderAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::RegisterPSOAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<PSOAsset> asset = std::make_shared<PSOAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::RegisterFontAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<FontAsset> asset = std::make_shared<FontAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::RegisterNavMeshAsset(const std::filesystem::path& aPath)
{
    std::shared_ptr<NavMeshAsset> asset = std::make_shared<NavMeshAsset>();
    asset->path = aPath;
    asset->name = Utilities::ToLowerCopy(aPath.filename().string());
    myAssets.emplace(asset->name, asset);
    return true;
}

bool AssetManager::ValidateAsset(const std::filesystem::path& aPath)
{
    if (std::filesystem::is_directory(aPath)) return false;

    if (!aPath.has_filename())
    {
        LOG(LogAssetManager, Error, "Path '{}' does not contain a filename!", aPath.string());
        return false;
    }

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

    return true;
}

std::shared_ptr<Asset> AssetManager::GetAssetBase(const std::filesystem::path& aPath)
{
    if (!IsAssetRegistered(aPath))
    {
        return std::shared_ptr<Asset>();
    }

    return myAssets[std::filesystem::path(Utilities::ToLowerCopy(aPath.filename().string()))];
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

bool AssetManager::RegisterEngineTextureAsset(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize)
{
    std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>();
    asset->texture = std::make_shared<Texture>();
    asset->name = Utilities::ToLowerCopy(aName.data());
    if (!GraphicsEngine::Get().LoadTexture(asset->name.string(), aTextureDataPtr, aTextureDataSize, *asset->texture))
    {
        LOG(LogAssetManager, Error, "Failed to register default texture asset {}", asset->name.string());
        return false;
    }
    
    asset->myIsLoaded = true;
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered default texture asset {}", asset->name.string());

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
    asset->name = "sm_planeprimitive";
    asset->myIsLoaded = true;

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
    asset->name = "sm_cubeprimitive";
    asset->myIsLoaded = true;
    
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
    asset->name = "sm_rampprimitive";
    asset->myIsLoaded = true;
    
    myAssets.emplace(asset->name, asset);

    LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->name.string());
    return true;
}

bool AssetManager::FilenameHasPrefix(const std::filesystem::path& aPath, const char* aPrefixCompare) const
{
    std::string filename = Utilities::ToLowerCopy(aPath.filename().string());
    std::string prefix = Utilities::ToLowerCopy(aPrefixCompare);
    return filename.starts_with(prefix);
}

bool AssetManager::FilenameHasExtension(const std::filesystem::path& aPath, const char* aExtensionCompare) const
{
    std::string filenameExtension = Utilities::ToLowerCopy(aPath.extension().string());
    std::string extension = Utilities::ToLowerCopy(aExtensionCompare);
    return filenameExtension == extension;
}

void AssetManager::LogAssetLoadError(const std::filesystem::path& aPath)
{
    LOG(LogAssetManager, Error, "Asset manager can not find asset at path: {}", aPath.string());
    MessageBox(NULL, L"Asset manager can not find asset, Please check the log for more information!", L"Asset Manager Error", MB_ICONERROR);
}