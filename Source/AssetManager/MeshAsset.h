#pragma once
#include <memory>
#include "Asset.h"
#include "Objects/Mesh.h"

class MeshAsset : public Asset
{
public:
	std::shared_ptr<Mesh> mesh;

	bool Load() override;
	bool Unload() override;
};

