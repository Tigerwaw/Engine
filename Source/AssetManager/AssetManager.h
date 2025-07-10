#pragma once
#include <cassert>
#include <chrono>
#include "Asset.h"
#include "WinPixEventRuntime/pix3.h"
#include "CommonUtilities/StringUtilities.hpp"

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

	template<typename T>
	std::shared_ptr<T> GetAsset(const std::filesystem::path& aPath);

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
	bool ValidateAsset(const std::filesystem::path& aPath);

	std::shared_ptr<Asset> GetAssetBase(const std::filesystem::path& aPath);

	void RegisterAllAssetsInDirectory();
	void LoadAllRegisteredAssets();

	// All assets that are required for the engine to function should be part of the .exe.
	void RegisterEngineAssets();
	bool RegisterEngineTextureAsset(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize);

	// Move to Graphics Engine
	bool RegisterPlanePrimitive();
	bool RegisterCubePrimitive();
	bool RegisterRampPrimitive();

	// Make Generic
	bool RegisterMeshAsset(const std::filesystem::path& aPath);
	bool RegisterAnimationAsset(const std::filesystem::path& aPath);
	bool RegisterMaterialAsset(const std::filesystem::path& aPath);
	bool RegisterTextureAsset(const std::filesystem::path& aPath);
	bool RegisterShaderAsset(const std::filesystem::path& aPath);
	bool RegisterPSOAsset(const std::filesystem::path& aPath);
	bool RegisterFontAsset(const std::filesystem::path& aPath);
	bool RegisterNavMeshAsset(const std::filesystem::path& aPath);
	
	bool FilenameHasPrefix(const std::filesystem::path& aPath, const char* aPrefixCompare) const;
	bool FilenameHasExtension(const std::filesystem::path& aPath, const char* aExtensionCompare) const;

	void LogAssetLoadError(const std::filesystem::path& aPath);
	
	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> myAssets;
	std::filesystem::path myContentRoot;
};

template<typename T>
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