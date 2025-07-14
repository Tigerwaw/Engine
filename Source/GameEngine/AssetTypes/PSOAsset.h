#pragma once
#include <memory>
#include "Asset.h"
#include "Objects/PipelineStateObject.h"

class PSOAsset : public Asset
{
public:
	std::shared_ptr<PipelineStateObject> pso;

	bool Load() override;
	bool Unload() override;
	int GetRefCount() override { return static_cast<int>(pso.use_count()); }
};

