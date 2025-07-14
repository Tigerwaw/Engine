#pragma once
#include <memory>
#include "Asset.h"
#include "Objects/Texture.h"

class TextureAsset : public Asset
{
public:
	std::shared_ptr<Texture> texture;

	bool Load() override;
	bool Unload() override;
	int GetRefCount() override { return static_cast<int>(texture.use_count()); }
};