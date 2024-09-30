#pragma once
#include "Objects/DynamicVertexBuffer.h"
#include "Objects/Vertices/ParticleVertex.h"
#include "ParticleEmitterSettings.h"

class ParticleSystem;

class ParticleEmitter
{
	friend class GraphicsEngine;
	friend class ParticleSystem;

	public:
		ParticleEmitter();
		virtual ~ParticleEmitter();
		void Update(float aDeltaTime);

		void SetMaterial(std::shared_ptr<Material> aMaterial) { myMaterial = aMaterial; }
		void SetSettings(ParticleEmitterSettings aParticleEmitterSettings) { mySettings = aParticleEmitterSettings; }

		std::shared_ptr<Material> GetMaterial() { return myMaterial; }
		ParticleEmitterSettings& GetEmitterSettings() { return mySettings; }
	protected:
		virtual void InitParticle(ParticleVertex& aParticle, size_t aIndex);
		virtual void UpdateParticle(ParticleVertex& aParticle, float aDeltaTime);
		virtual void InitInternal();
		std::vector<ParticleVertex> myParticles;
	private:
		std::shared_ptr<Material> myMaterial;
		DynamicVertexBuffer myVertexBuffer;
		ParticleEmitterSettings mySettings;
};