#pragma once
#include <cassert>
#include <chrono>
#include "Asset.h"
#include "WinPixEventRuntime/pix3.h"
#include "CommonUtilities/StringUtilities.hpp"
#include <functional>

// TEMP INCLUDES
#include "MeshAsset.h"
#include "NavMeshAsset.h"
#include "AnimationAsset.h"
#include "FontAsset.h"
#include "MaterialAsset.h"
#include "PSOAsset.h"
#include "ShaderAsset.h"
#include "TextureAsset.h"

class AssetManager
{
public:
	static AssetManager& Get()
	{
		static AssetManager myInstance;
		return myInstance;
	}

	template<typename T> requires std::is_base_of_v<Asset, T>
	std::shared_ptr<T> GetAsset(const std::filesystem::path& aPath);

	template<typename T>  requires std::is_base_of_v<Asset, T>
	bool RegisterAsset(const std::filesystem::path& aPath);

	bool RegisterAsset(const std::filesystem::path& aPath);
	bool DeregisterAsset(const std::filesystem::path& aPath);
	bool IsAssetRegistered(const std::filesystem::path& aPath);

	bool LoadAsset(const std::filesystem::path& aPath);
	bool UnloadAsset(const std::filesystem::path& aPath);

	bool Initialize(const std::filesystem::path& aContentRootPath, bool aAutoRegisterAllAssetsInRoot = true);
	std::filesystem::path& GetContentRoot() { return myContentRoot; }
	std::filesystem::path MakeRelative(const std::filesystem::path& aPath) const;
private:
	AssetManager();
	~AssetManager();
	bool ValidateAssetPath(const std::filesystem::path& aPath);

	std::shared_ptr<Asset> GetAssetBase(const std::filesystem::path& aPath);

	void RegisterAllAssetsInDirectory();
	void LoadAllRegisteredAssets();

	// All assets that are required for the engine to function should be part of the .exe.
	void RegisterEngineAssets();
	bool RegisterEngineTextureAsset(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize);
	
	bool FilenameHasPrefix(const std::filesystem::path& aPath, const char* aPrefixCompare) const;
	bool FilenameHasExtension(const std::filesystem::path& aPath, const char* aExtensionCompare) const;

	void LogAssetLoadError(const std::filesystem::path& aPath);
	
	std::filesystem::path myContentRoot;
	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> myAssets;
	std::unordered_map<std::string, std::function<bool(const std::string&, const std::filesystem::path&)>> myFileExtensionToRegisterFunc;
};

template<typename T> requires std::is_base_of_v<Asset, T>
inline std::shared_ptr<T> AssetManager::GetAsset(const std::filesystem::path& aPath)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "AssetManager Get Asset");
	std::shared_ptr<T> asset = nullptr;
	
	if (IsAssetRegistered(aPath))
	{
		std::shared_ptr<Asset> assetBase = GetAssetBase(aPath);
		if (assetBase->IsLoaded())
		{
			asset = std::dynamic_pointer_cast<T>(assetBase);
			return asset;
		}
		else
		{
			LoadAsset(aPath);
			asset = std::dynamic_pointer_cast<T>(assetBase);
			return asset;
		}
	}
	else
	{
		LogAssetLoadError(aPath);
	}

	return nullptr;
}

template<typename T>  requires std::is_base_of_v<Asset, T>
bool AssetManager::RegisterAsset(const std::filesystem::path& aPath)
{
	std::shared_ptr<T> asset = std::make_shared<T>();
	asset->myPath = aPath;
	asset->myName = Utilities::ToLowerCopy(aPath.filename().string());

	myAssets.emplace(asset->myName, asset);
	return true;
}