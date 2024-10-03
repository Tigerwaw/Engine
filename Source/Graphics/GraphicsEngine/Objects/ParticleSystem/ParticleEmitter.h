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
		std::shared_ptr<Material> GetMaterial() { return myMaterial; }

		template<typename EmitterSettingsType>
		std::enable_if_t<std::is_base_of_v<ParticleEmitterSettings, EmitterSettingsType>, void>
		Init(const EmitterSettingsType& aSettings)
		{
			mySettings = new EmitterSettingsType();
			*mySettings = aSettings;
			InitInternal();
		}

		template<typename EmitterSettingsType>
		FORCEINLINE std::enable_if_t<std::is_base_of_v<ParticleEmitterSettings, EmitterSettingsType>, EmitterSettingsType* const>
		GetEmitterSettings() const
		{
			return dynamic_cast<EmitterSettingsType const*>(mySettings);
		}

	protected:
		virtual void InitParticle(ParticleVertex& aParticle, size_t aIndex);
		virtual void UpdateParticle(ParticleVertex& aParticle, float aDeltaTime);
		virtual void InitInternal();
		std::vector<ParticleVertex> myParticles;
	private:
		DynamicVertexBuffer myVertexBuffer;
		std::shared_ptr<Material> myMaterial;
		ParticleEmitterSettings* mySettings = nullptr;
};