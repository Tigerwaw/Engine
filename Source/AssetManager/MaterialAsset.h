#pragma once
#include <memory>
#include "Asset.h"
#include "Objects/Material.h"

class MaterialAsset : public Asset
{
public:
	std::shared_ptr<Material> material;

	//bool Load() override;
	//bool Unload() override;
};

