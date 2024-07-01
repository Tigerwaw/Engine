#pragma once
#include "GraphicsCommandBase.h"
#include <memory>
#include <vector>
#include <array>
#include "GameEngine/Math/Matrix4x4.hpp"

namespace CU = CommonUtilities;

class Mesh;
class AnimatedModel;
class Material;

struct RenderAnimatedMesh : GraphicsCommandBase
{
public:
	RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> mesh;
	CU::Matrix4x4f transform;
	std::array<CU::Matrix4x4f, 128> jointTransforms;
	std::vector<std::shared_ptr<Material>> materialList;
};

