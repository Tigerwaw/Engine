#pragma once

class Asset
{
public:
	virtual ~Asset() = default;

	const std::filesystem::path& GetPath() { return myPath; }
	const std::filesystem::path& GetName() { return myName; }

	bool IsLoaded() const { return myIsLoaded; }
	virtual bool Load() = 0;
	virtual bool Unload() = 0;
protected:
	bool myIsLoaded = false;
private:
	std::filesystem::path myPath;
	std::filesystem::path myName;

	friend class AssetManager;
};