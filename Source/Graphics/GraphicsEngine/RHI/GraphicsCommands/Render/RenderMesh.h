#pragma once
#include "GraphicsCommandBase.h"
#include <memory>
#include <vector>
#include "GameEngine/Math/Matrix4x4.hpp"

namespace CU = CommonUtilities;

class Mesh;
class Material;
class Model;

struct RenderMesh : GraphicsCommandBase
{
public:
	RenderMesh(std::shared_ptr<Model> aModel);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> mesh;
	CU::Matrix4x4f transform;
	std::vector<std::shared_ptr<Material>> materialList;
};

