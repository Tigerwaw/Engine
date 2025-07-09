#pragma once

class Asset
{
public:
	virtual ~Asset() = default;
	std::filesystem::path path;
	std::filesystem::path name;

	//virtual bool Load() = 0;
	//virtual bool Unload() = 0;
	//virtual const char* GetAcceptedExtensionsOrWhatever
};