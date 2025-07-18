#pragma once
#include <memory>
#include "Asset.h"
#include "Objects/Animation.h"

class AnimationAsset : public Asset
{
public:
	std::shared_ptr<Animation> animation;

	bool Load() override;
	bool Unload() override;
	int GetRefCount() override { return static_cast<int>(animation.use_count()); }
};

