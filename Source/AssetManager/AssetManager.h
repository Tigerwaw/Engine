#pragma once
#include <cassert>
#include <future>
#include <chrono>
#include "Asset.h"
#include "WinPixEventRuntime/pix3.h"
#include "CommonUtilities/StringUtilities.hpp"

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

	bool DeregisterAsset(const std::filesystem::path& aPath);
	bool DeregisterAsset(const std::shared_ptr<Asset> aAsset);

	bool RegisterAsset(const std::filesystem::path& aPath);

	bool Initialize(const std::filesystem::path& aContentRootPath, bool aAutoRegisterAllAssetsInRoot = true);
	bool RegisterMeshAsset(const std::filesystem::path& aPath);
	bool RegisterAnimationAsset(const std::filesystem::path& aPath);
	bool RegisterMaterialAsset(const std::filesystem::path& aPath);
	bool RegisterTextureAsset(const std::filesystem::path& aPath);
	bool RegisterShaderAsset(const std::filesystem::path& aPath);
	bool RegisterPSOAsset(const std::filesystem::path& aPath);
	bool RegisterFontAsset(const std::filesystem::path& aPath);
	bool RegisterNavMeshAsset(const std::filesystem::path& aPath);
	std::filesystem::path& GetContentRoot() { return myContentRoot; }
	std::filesystem::path MakeRelative(const std::filesystem::path& aPath) const;
private:
	AssetManager();
	~AssetManager();
	bool ValidateAsset(const std::filesystem::path& aPath);

	void RegisterAllAssetsInDirectory();
	void RegisterEngineAssets();
	bool RegisterEngineTextureAsset(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize);
	bool RegisterPlanePrimitive();
	bool RegisterCubePrimitive();
	bool RegisterRampPrimitive();
	
	bool FilenameHasPrefix(const std::filesystem::path& aPath, const char* aPrefixCompare) const;
	bool FilenameHasExtension(const std::filesystem::path& aPath, const char* aExtensionCompare) const;

	void LogAssetLoadError(const std::filesystem::path& aPath);

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> myAssets;
	std::filesystem::path myContentRoot;

	std::vector<std::future<bool>> myFutures;
	std::mutex myAssetMutex;
	std::mutex myImporterMutex;
};

template<typename T>
inline std::shared_ptr<T> AssetManager::GetAsset(const std::filesystem::path& aPath)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "AssetManager Get Asset");
	std::shared_ptr<T> asset = nullptr;
	
	std::filesystem::path filename(Utilities::ToLowerCopy(aPath.filename().string()));
	if (myAssets.contains(filename))
	{
		asset = std::dynamic_pointer_cast<T>(myAssets.at(filename));
	}
	else
	{
		LogAssetLoadError(filename);
		assert(myAssets.contains(filename));
		return nullptr;
	}

	return asset;
}