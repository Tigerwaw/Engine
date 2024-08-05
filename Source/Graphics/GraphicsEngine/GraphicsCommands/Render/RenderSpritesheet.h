#pragma once
#include "GraphicsCommandBase.h"

#include <memory>
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"

namespace CU = CommonUtilities;

class Spritesheet;
class Texture;
class Material;

struct RenderSpritesheet : GraphicsCommandBase
{
public:
	RenderSpritesheet(std::shared_ptr<Spritesheet> aSprite);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Material> material;
	std::shared_ptr<Texture> texture;
	CU::Matrix4x4f matrix;
	CU::Vector2f sheetDimensions;
	float currentFrame;
};

