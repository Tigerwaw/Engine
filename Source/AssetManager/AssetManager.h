#pragma once
#include <cassert>
#include <future>
#include <chrono>
#include "Asset.h"

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
	std::filesystem::path MakeRelative(const std::filesystem::path& aPath);
	bool DoesAssetExist(const std::filesystem::path& aPath);
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

	void LogAssetLoadError(const std::filesystem::path& aPath);

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> myAssets;
	std::filesystem::path myContentRoot;

	std::vector<std::future<bool>> myFutures;
};

template<typename T>
inline std::shared_ptr<T> AssetManager::GetAsset(const std::filesystem::path& aPath)
{
	std::shared_ptr<T> asset = nullptr;

	if (myAssets.contains(aPath.stem()))
	{
		asset = std::dynamic_pointer_cast<T>(myAssets.at(aPath.stem()));
	}
	else
	{
		LogAssetLoadError(aPath);
		assert(myAssets.contains(aPath));
		return nullptr;
	}

	return asset;
}