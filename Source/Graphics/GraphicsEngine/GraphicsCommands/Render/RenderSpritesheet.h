#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"



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
	Math::Matrix4x4f matrix;
	Math::Vector2f sheetDimensions;
	float currentFrame;
};

