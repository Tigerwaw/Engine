#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;

class RenderDebugMesh : GraphicsCommandBase
{
public:
	struct DebugMeshRenderData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
		std::vector<std::shared_ptr<Material>> materialList;
	};
	RenderDebugMesh(const DebugMeshRenderData& aModelData);
	void Execute() override;
	void Destroy() override;
private:
	DebugMeshRenderData myData;
};