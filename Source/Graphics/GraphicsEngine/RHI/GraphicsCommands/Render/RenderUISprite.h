#pragma once
#include "GraphicsCommandBase.h"

#include <memory>
#include "GameEngine/Math/Vector.hpp"

namespace CU = CommonUtilities;

class Sprite;
class SpriteObject;
class Texture;

struct RenderUISprite : GraphicsCommandBase
{
public:
	RenderUISprite(std::shared_ptr<SpriteObject> aSpriteObject);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Sprite> sprite;
	std::shared_ptr<Texture> texture;
	CU::Vector4f position;
	CU::Vector2f size;
};

