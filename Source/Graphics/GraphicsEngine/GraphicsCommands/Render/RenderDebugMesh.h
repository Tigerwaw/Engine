#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"
#include "GameEngine/Math/Matrix4x4.hpp"

namespace CU = CommonUtilities;

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
	CU::Matrix4x4f transform;
	std::vector<std::shared_ptr<Material>> materialList;
};