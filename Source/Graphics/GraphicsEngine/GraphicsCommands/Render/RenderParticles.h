#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"


class ParticleSystem;

class RenderParticles : public GraphicsCommandBase
{
public:
	RenderParticles(std::shared_ptr<ParticleSystem> aParticleSystem);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<ParticleSystem> particleSystem;
	Math::Matrix4x4f transform;
};

