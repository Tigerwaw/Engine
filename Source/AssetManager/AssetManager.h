#pragma once
#include <cassert>
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

	bool Initialize(const std::filesystem::path& aContentRootPath, bool aAutoRegisterAllAssetsInRoot = true);
	bool RegisterMeshAsset(const std::filesystem::path& aPath);
	bool RegisterAnimationAsset(const std::filesystem::path& aPath);
	bool RegisterMaterialAsset(const std::filesystem::path& aPath);
	bool RegisterTextureAsset(const std::filesystem::path& aPath);
	std::filesystem::path& GetContentRoot() { return myContentRoot; }
private:
	AssetManager();
	~AssetManager();
	bool ValidateAsset(const std::filesystem::path& aPath);
	std::filesystem::path MakeRelative(const std::filesystem::path& aPath);

	void RegisterAllAssetsInDirectory();
	void RegisterEngineAssets();
	bool RegisterEngineTextureAsset(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize);
	bool RegisterPlanePrimitive();
	bool RegisterCubePrimitive();
	bool RegisterRampPrimitive();

	std::unordered_map<std::filesystem::path, std::shared_ptr<Asset>> myAssets;
	std::filesystem::path myContentRoot;
};

template<typename T>
inline std::shared_ptr<T> AssetManager::GetAsset(const std::filesystem::path& aPath)
{
	std::shared_ptr<T> asset = nullptr;

	if (myAssets.contains(aPath))
	{
		asset = std::dynamic_pointer_cast<T>(myAssets.at(aPath));
	}
	else
	{
		assert(myAssets.contains(aPath) && std::string("Asset manager can not find asset at path: " + aPath.string()).c_str());
		return nullptr;
	}

	return asset;
}
