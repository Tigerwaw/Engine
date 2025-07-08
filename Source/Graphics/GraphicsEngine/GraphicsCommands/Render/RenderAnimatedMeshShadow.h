#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;

class RenderAnimatedMeshShadow : GraphicsCommandBase
{
public:
	struct AnimMeshShadowRenderData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
		std::array<Math::Matrix4x4f, 128> jointTransforms;
	};

	RenderAnimatedMeshShadow(const AnimMeshShadowRenderData& aModelData);
	RenderAnimatedMeshShadow(AnimMeshShadowRenderData&& aModelData);
	void Execute() override;
	void Destroy() override;
private:
	AnimMeshShadowRenderData myData;
};

