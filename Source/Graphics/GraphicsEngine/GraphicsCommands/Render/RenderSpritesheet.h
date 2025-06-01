#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

class Spritesheet;
class Texture;
class Material;

class RenderSpritesheet : GraphicsCommandBase
{
public:
	struct SpritesheetData
	{
		std::shared_ptr<Material> material;
		std::shared_ptr<Texture> texture;
		Math::Matrix4x4f matrix;
		Math::Vector2f sheetDimensions;
		float currentFrame;
	};

	RenderSpritesheet(const SpritesheetData& aSpriteData);
	void Execute() override;
	void Destroy() override;
private:
	SpritesheetData myData;
};

