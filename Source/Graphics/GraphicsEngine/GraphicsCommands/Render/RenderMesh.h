#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
class Model;
struct PipelineStateObject;

struct RenderMesh : GraphicsCommandBase
{
public:
	RenderMesh(std::shared_ptr<Model> aModel);
	RenderMesh(std::shared_ptr<Model> aModel, std::shared_ptr<PipelineStateObject> aPSOoverride);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> mesh;
	Math::Matrix4x4f transform;
	std::vector<std::shared_ptr<Material>> materialList;
	std::shared_ptr<PipelineStateObject> psoOverride;
	Math::Vector4f customShaderParams_1;
	Math::Vector4f customShaderParams_2;
};

