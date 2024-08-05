#pragma once
#include "GraphicsCommandBase.h"

#include <memory>
#include "GameEngine/Math/Matrix.hpp"

namespace CU = CommonUtilities;

class Sprite;
class Texture;
class Material;

struct RenderSprite : GraphicsCommandBase
{
public:
	RenderSprite(std::shared_ptr<Sprite> aSprite);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Material> material;
	std::shared_ptr<Texture> texture;
	CU::Matrix4x4f matrix;
};

