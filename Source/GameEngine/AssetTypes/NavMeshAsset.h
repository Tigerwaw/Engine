#pragma once
#include <memory>
#include "Asset.h"
#include "Pathfinding/NavMesh.h"

class NavMeshAsset : public Asset
{
public:
	std::shared_ptr<NavMesh> navmesh;

	bool Load() override;
	bool Unload() override;
	int GetRefCount() override { return static_cast<int>(navmesh.use_count()); }
};

