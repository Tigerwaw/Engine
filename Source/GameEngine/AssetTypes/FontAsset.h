#pragma once
#include <memory>
#include "Asset.h"
#include "Objects/Text/Font.h"

class FontAsset : public Asset
{
public:
	std::shared_ptr<Font> font;

	bool Load() override;
	bool Unload() override;
};

