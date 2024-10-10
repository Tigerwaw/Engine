#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"

class Mesh;
class Material;
class InstancedModel;
struct PipelineStateObject;

class RenderInstancedMesh : public GraphicsCommandBase
{
public:
	RenderInstancedMesh(std::shared_ptr<InstancedModel> aInstancedModel);
	RenderInstancedMesh(std::shared_ptr<InstancedModel> aInstancedModel, std::shared_ptr<PipelineStateObject> aPSOoverride);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> mesh;
	CU::Matrix4x4f transform;
	std::vector<std::shared_ptr<Material>> materialList;
	std::shared_ptr<PipelineStateObject> psoOverride;
	DynamicVertexBuffer* instanceBuffer = nullptr;
	unsigned meshCount;
};

