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
struct PipelineStateObject;

struct RenderAnimatedMesh : GraphicsCommandBase
{
public:
	RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel);
	RenderAnimatedMesh(std::shared_ptr<AnimatedModel> aModel, std::shared_ptr<PipelineStateObject> aPSOoverride);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> mesh;
	CU::Matrix4x4f transform;
	std::array<CU::Matrix4x4f, 128> jointTransforms;
	std::vector<std::shared_ptr<Material>> materialList;
	std::shared_ptr<PipelineStateObject> psoOverride;
};

