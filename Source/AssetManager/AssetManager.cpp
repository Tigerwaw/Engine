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

    myFileExtensionToRegisterFunc[".fbx"] = std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string& aFilename, const std::filesystem::path& aPath)
        {
            if (aFilename.starts_with("sm") || aFilename.starts_with("sk"))
            {
                return AssetManager::Get().RegisterAsset<MeshAsset>(aPath);
            }
            else if (aFilename.starts_with("a"))
            {
                return AssetManager::Get().RegisterAsset<AnimationAsset>(aPath);
            }
            else if (aFilename.starts_with("nm"))
            {
                return AssetManager::Get().RegisterAsset<NavMeshAsset>(aPath);
            }

            return false;
        });

    myFileExtensionToRegisterFunc[".mat"] = std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<MaterialAsset>(aPath);
        });

    myFileExtensionToRegisterFunc[".dds"] = std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<TextureAsset>(aPath);
        });

    myFileExtensionToRegisterFunc[".cso"] = std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<ShaderAsset>(aPath);
        });

    myFileExtensionToRegisterFunc[".pso"] = std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<PSOAsset>(aPath);
        });

    myFileExtensionToRegisterFunc[".font"] = std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<FontAsset>(aPath);
        });

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
        asset->myName = "defaultmaterial";
        asset->myIsLoaded = true;
        myAssets.emplace(asset->myName, asset);
        LOG(LogAssetManager, Log, "Registered material asset {}", asset->myName.string());
    }

    {
        std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
        asset->mesh = std::make_shared<Mesh>(GraphicsEngine::Get().GetResourceVendor().CreatePlanePrimitive());
        asset->myName = "sm_planeprimitive";
        asset->myIsLoaded = true;
        myAssets.emplace(asset->myName, asset);
        LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->myName.string());
    }

    {
        std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
        asset->mesh = std::make_shared<Mesh>(GraphicsEngine::Get().GetResourceVendor().CreateCubePrimitive());
        asset->myName = "sm_cubeprimitive";
        asset->myIsLoaded = true;
        myAssets.emplace(asset->myName, asset);
        LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->myName.string());
    }
    
    {
        std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
        asset->mesh = std::make_shared<Mesh>(GraphicsEngine::Get().GetResourceVendor().CreateRampPrimitive());
        asset->myName = "sm_rampprimitive";
        asset->myIsLoaded = true;
        myAssets.emplace(asset->myName, asset);
        LOG(LogAssetManager, Log, "Registered mesh asset {}", asset->myName.string());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Models/"))
    {
        if (!ValidateAssetPath(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".fbx")) continue;
        RegisterAsset<MeshAsset>(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Textures/"))
    {
        if (!ValidateAssetPath(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".dds")) continue;
        RegisterAsset<TextureAsset>(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Shaders/"))
    {
        if (!ValidateAssetPath(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".cso")) continue;
        RegisterAsset<ShaderAsset>(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/PSOs/"))
    {
        if (!ValidateAssetPath(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".pso")) continue;
        RegisterAsset<PSOAsset>(file.path());
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot / "EngineAssets/Materials/"))
    {
        if (!ValidateAssetPath(file.path())) continue;
        if (!FilenameHasExtension(file.path(), ".mat")) continue;
        RegisterAsset<MaterialAsset>(file.path());
    }
}

void AssetManager::RegisterAllAssetsInDirectory()
{
    std::chrono::system_clock::time_point registerAllStartTime = std::chrono::system_clock::now();

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot))
    {
        if (!ValidateAssetPath(file.path())) continue;

        std::chrono::system_clock::time_point registerStartTime = std::chrono::system_clock::now();

        std::string extension = Utilities::ToLowerCopy(file.path().extension().string());
        std::string name = Utilities::ToLowerCopy(file.path().filename().string());
        if (myFileExtensionToRegisterFunc.contains(extension))
        {
            if (myFileExtensionToRegisterFunc.at(extension)(name, file.path()))
            {
                std::chrono::duration<float, std::ratio<1, 1000>> registerTime = std::chrono::system_clock::now() - registerStartTime;
                LOG(LogAssetManager, Log, "Registered asset {} in {}ms", name, std::round(registerTime.count()));
            }
            else
            {
                LOG(LogAssetManager, Error, "Failed to register asset {}", name);
            }
        }
        else
        {
            LOG(LogAssetManager, Error, "Extension {} has not been registered to an asset type!", extension);
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

bool AssetManager::ValidateAssetPath(const std::filesystem::path& aPath)
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
    asset->myName = Utilities::ToLowerCopy(aName.data());
    if (!GraphicsEngine::Get().GetResourceVendor().LoadTexture(asset->myName.string(), aTextureDataPtr, aTextureDataSize, *asset->texture))
    {
        LOG(LogAssetManager, Error, "Failed to register default texture asset {}", asset->myName.string());
        return false;
    }
    
    asset->myIsLoaded = true;
    myAssets.emplace(asset->myName, asset);

    LOG(LogAssetManager, Log, "Registered default texture asset {}", asset->myName.string());

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