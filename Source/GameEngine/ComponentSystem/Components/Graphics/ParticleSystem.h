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

	template<typename EmitterType, typename EmitterSettingsType>
	EmitterType* AddEmitter(const EmitterSettingsType& aSettings);

	const std::vector<ParticleEmitter*>& GetEmitters() const { return myEmitters; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	std::vector<ParticleEmitter*> myEmitters;
};

template<typename EmitterType, typename EmitterSettingsType>
inline EmitterType* ParticleSystem::AddEmitter(const EmitterSettingsType& aSettings)
{
	EmitterType* emitter = new EmitterType();
	emitter->Init(aSettings);
	myEmitters.emplace_back(emitter);
	return emitter;
}
