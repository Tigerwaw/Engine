#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"
#include "Objects/ParticleSystem/ParticleEmitter.h"

class RenderParticles : public GraphicsCommandBase
{
public:
	struct RenderParticlesData
	{
		std::vector<ParticleEmitter> emitters;
		Math::Matrix4x4f transform;
	};

	RenderParticles(const RenderParticlesData& aParticleSystemData);
	void Execute() override;
	void Destroy() override;
private:
	RenderParticlesData myData;
};

