#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"



class Mesh;
class Material;
class DebugModel;

struct RenderDebugMesh : GraphicsCommandBase
{
public:
	RenderDebugMesh(std::shared_ptr<DebugModel> aModel);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> mesh;
	Math::Matrix4x4f transform;
	std::vector<std::shared_ptr<Material>> materialList;
};