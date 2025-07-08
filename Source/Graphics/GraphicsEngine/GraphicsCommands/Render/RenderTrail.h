#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"
#include "Objects/ParticleSystem/TrailEmitter.h"

class RenderTrail : public GraphicsCommandBase
{
public:
	struct TrailData
	{
		std::vector<TrailEmitter> emitters;
		Math::Matrix4x4f transform;
	};

	RenderTrail(const TrailData& aTrailData);
	RenderTrail(TrailData&& aTrailData);
	void Execute() override;
	void Destroy() override;
private:
	TrailData myData;
};

