#pragma once
#include "GraphicsCommandBase.h"

#include <memory>
#include "GameEngine/Math/Vector.hpp"

namespace CU = CommonUtilities;

class Sprite;
class Texture;

struct RenderSprite : GraphicsCommandBase
{
public:
	RenderSprite(std::shared_ptr<Sprite> aSprite);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Texture> texture;
	CU::Vector4f position;
	CU::Vector2f size;
};

