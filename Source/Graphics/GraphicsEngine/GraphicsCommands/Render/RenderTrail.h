#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"


class TrailSystem;

class RenderTrail : public GraphicsCommandBase
{
public:
	RenderTrail(std::shared_ptr<TrailSystem> aTrailSystem);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<TrailSystem> trailSystem;
	Math::Matrix4x4f transform;
};

