#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderInstancedMeshDebugPass : public GraphicsCommandBase
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

	RenderInstancedMeshDebugPass(const InstancedMeshRenderData& aInstancedModelData);
	RenderInstancedMeshDebugPass(InstancedMeshRenderData&& aInstancedModelData);
	void Execute() override;
	void Destroy() override;
private:
	InstancedMeshRenderData myData;
};

