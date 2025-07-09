#pragma once
#include <memory>
#include "Asset.h"
#include "Objects/Shader.h"

class ShaderAsset : public Asset
{
public:
	std::shared_ptr<Shader> shader;

	//bool Load() override;
	//bool Unload() override;
};

