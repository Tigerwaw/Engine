#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;

class RenderMeshShadow : GraphicsCommandBase
{
public:
	struct RenderMeshShadowData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
	};

	RenderMeshShadow(const RenderMeshShadowData& aModelData);
	RenderMeshShadow(RenderMeshShadowData&& aModelData);
	void Execute() override;
	void Destroy() override;
private:
	RenderMeshShadowData myData;
};

