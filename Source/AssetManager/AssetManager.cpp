#include "Assetpch.h"

#include "AssetManager.h"
#include "Asset.h"
#include "GraphicsEngine.h"
#include "Objects/Vertices/Vertex.h"

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

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
}

void AssetManager::RegisterAllAssetsInDirectory()
{
    std::chrono::system_clock::time_point registerAllStartTime = std::chrono::system_clock::now();

    for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot))
    {
        if (!ValidateAssetPath(file.path())) continue;

        std::string extension = Utilities::ToLowerCopy(file.path().extension().string());
        std::string name = Utilities::ToLowerCopy(file.path().filename().string());
        if (myFileExtensionToRegisterFunc.contains(extension))
        {
            if (myFileExtensionToRegisterFunc.at(extension)(name, file.path()))
            {
                LOG(LogAssetManager, Log, "Registered asset {}", name);
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