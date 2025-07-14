#include "Assetpch.h"

#include "AssetManager.h"
#include "Asset.h"
#include "GraphicsEngine.h"
#include "Objects/Vertices/Vertex.h"

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

bool AssetManager::Initialize(const std::filesystem::path& aContentRootPath, bool aAutoRegisterAllAssetsInRoot)
{
    LOG(LogAssetManager, Log, "Initializing Asset Manager...");

    myLastClearUnusedAssetsTimepoint = std::chrono::system_clock::now();

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

    if (aAutoRegisterAllAssetsInRoot)
    {
        RegisterAllAssetsInDirectory();
    }

    LOG(LogAssetManager, Log, "Asset Manager Initialized! Root content directory set to '{}'.", myContentRoot.string());
    return true;
}

void AssetManager::Update()
{
    std::chrono::duration<float> elapsedTime = std::chrono::system_clock::now() - myLastClearUnusedAssetsTimepoint;
    if (elapsedTime.count() > myTimeBetweenUnusedAssetClears)
    {
        myLastClearUnusedAssetsTimepoint = std::chrono::system_clock::now();
        ClearUnusedAssets();
    }
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

AssetManager::AssetManager()
{
    TGA::FBX::Importer::InitImporter();
}

AssetManager::~AssetManager()
{
    TGA::FBX::Importer::UninitImporter();
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

std::shared_ptr<Asset> AssetManager::GetAssetBase(const std::filesystem::path& aPath)
{
    if (!IsAssetRegistered(aPath))
    {
        return std::shared_ptr<Asset>();
    }

    return myAssets[std::filesystem::path(Utilities::ToLowerCopy(aPath.filename().string()))];
}

bool AssetManager::LoadAsset(const std::filesystem::path& aPath)
{
    if (auto assetBase = GetAssetBase(aPath))
    {
        if (assetBase->Load())
        {
            LOG(LogAssetManager, Log, "Loaded asset {}", aPath.string());
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
            LOG(LogAssetManager, Log, "Unloaded asset {}", aPath.string());
            assetBase->myIsLoaded = false;
            return true;
        }
    }

    return false;
}

void AssetManager::LogAssetLoadError(const std::filesystem::path& aPath)
{
    LOG(LogAssetManager, Error, "Asset manager can not find asset at path: {}", aPath.string());
    MessageBox(NULL, L"Asset manager can not find asset, Please check the log for more information!", L"Asset Manager Error", MB_ICONERROR);
}

void AssetManager::ClearUnusedAssets()
{
    for (auto& [name, asset] : myAssets)
    {
        if (asset->IsLoaded())
        {
            if (asset->GetRefCount() == 1)
            {
                UnloadAsset(asset->GetName());
            }
        }
    }
}
