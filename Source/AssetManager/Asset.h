#pragma once

class Asset
{
public:
	virtual ~Asset() = default;
	std::filesystem::path path;
	std::filesystem::path name;

	bool IsLoaded() const { return myIsLoaded; }
	virtual bool Load() = 0;
	virtual bool Unload() = 0;
	//virtual const char* GetAcceptedExtensionsOrWhatever
private:
	bool myIsLoaded = false;

	friend class AssetManager;
};