#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderInstancedMesh : public GraphicsCommandBase
{
public:
	struct InstancedMeshRenderData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
		std::vector<std::shared_ptr<Material>> materialList;
		DynamicVertexBuffer* instanceBuffer = nullptr;
		unsigned meshCount = 0;
	};

	RenderInstancedMesh(const InstancedMeshRenderData& aInstancedModelData);
	RenderInstancedMesh(InstancedMeshRenderData&& aInstancedModelData);
	void Execute() override;
	void Destroy() override;
private:
	InstancedMeshRenderData myData;
};

