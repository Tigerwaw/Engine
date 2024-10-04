#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"
#include "GameEngine/Math/Matrix4x4.hpp"
namespace CU = CommonUtilities;

class TrailSystem;

class RenderTrail : public GraphicsCommandBase
{
public:
	RenderTrail(std::shared_ptr<TrailSystem> aTrailSystem);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<TrailSystem> trailSystem;
	CU::Matrix4x4f transform;
};

