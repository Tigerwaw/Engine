#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Graphics/GraphicsEngine/Objects/ParticleSystem/ParticleEmitter.h"

class ParticleSystem : public Component
{
public:
	~ParticleSystem() override;
	ParticleSystem() = default;

	void Start() override;
	void Update() override;

	void AddEmitter(ParticleEmitter aParticleEmitter);
	std::vector<ParticleEmitter>& GetEmitters() { return myEmitters; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	std::vector<ParticleEmitter> myEmitters;
};