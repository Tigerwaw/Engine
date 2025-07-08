#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class Mesh;

class RenderInstancedMeshShadow : public GraphicsCommandBase
{
public:
	struct InstancedMeshShadowRenderData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
		DynamicVertexBuffer* instanceBuffer = nullptr;
		unsigned meshCount = 0;
	};

	RenderInstancedMeshShadow(const InstancedMeshShadowRenderData& aInstancedModelData);
	RenderInstancedMeshShadow(InstancedMeshShadowRenderData&& aInstancedModelData);
	void Execute() override;
	void Destroy() override;
private:
	InstancedMeshShadowRenderData myData;
};

