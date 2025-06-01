#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix.hpp"

class Sprite;
class Texture;
class Material;

class RenderSprite : GraphicsCommandBase
{
public:
	struct SpriteData
	{
		std::shared_ptr<Material> material;
		std::shared_ptr<Texture> texture;
		Math::Matrix4x4f matrix;
	};

	RenderSprite(const SpriteData& aSpriteData);
	void Execute() override;
	void Destroy() override;
private:
	SpriteData myData;
};

