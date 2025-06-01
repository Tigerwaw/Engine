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
		std::shared_ptr<PipelineStateObject> psoOverride;
		DynamicVertexBuffer* instanceBuffer = nullptr;
		unsigned meshCount = 0;
	};

	RenderInstancedMesh(const InstancedMeshRenderData& aInstancedModelData);
	void Execute() override;
	void Destroy() override;
private:
	InstancedMeshRenderData myData;
};

