#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderMeshDebugPass : GraphicsCommandBase
{
public:
	struct RenderMeshData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
		std::vector<std::shared_ptr<Material>> materialList;
		Math::Vector4f customShaderParams_1;
		Math::Vector4f customShaderParams_2;
	};

	RenderMeshDebugPass(const RenderMeshData& aModelData);
	RenderMeshDebugPass(RenderMeshData&& aModelData);
	void Execute() override;
	void Destroy() override;
private:
	RenderMeshData myData;
};

