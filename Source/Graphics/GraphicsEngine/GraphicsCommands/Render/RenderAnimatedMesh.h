#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderAnimatedMesh : GraphicsCommandBase
{
public:
	struct AnimMeshRenderData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
		std::array<Math::Matrix4x4f, 128> jointTransforms;
		std::vector<std::shared_ptr<Material>> materialList;
		std::shared_ptr<PipelineStateObject> psoOverride;
	};

	RenderAnimatedMesh(const AnimMeshRenderData& aModelData);
	RenderAnimatedMesh(AnimMeshRenderData&& aModelData);
	void Execute() override;
	void Destroy() override;
private:
	AnimMeshRenderData myData;
};

