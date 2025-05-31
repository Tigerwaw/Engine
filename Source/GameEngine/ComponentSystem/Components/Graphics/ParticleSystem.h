#pragma once
#include "ComponentSystem/Component.h"
#include "Objects/ParticleSystem/ParticleEmitter.h"

class ParticleSystem : public Component
{
public:
	~ParticleSystem() override;
	ParticleSystem() = default;

	void Start() override;
	void Update() override;

	ParticleEmitter& AddEmitter(const ParticleEmitterSettings& aSettings);

	std::vector<ParticleEmitter>& GetEmitters() { return myEmitters; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	std::vector<ParticleEmitter> myEmitters;
};
