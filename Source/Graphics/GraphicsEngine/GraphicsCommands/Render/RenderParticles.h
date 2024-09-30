#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"
#include "GameEngine/Math/Matrix4x4.hpp"
namespace CU = CommonUtilities;

class ParticleSystem;

class RenderParticles : public GraphicsCommandBase
{
public:
	RenderParticles(std::shared_ptr<ParticleSystem> aParticleSystem);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<ParticleSystem> particleSystem;
	CU::Matrix4x4f transform;
};

