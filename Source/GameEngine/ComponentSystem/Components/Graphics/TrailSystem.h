#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Graphics/GraphicsEngine/Objects/ParticleSystem/TrailEmitter.h"

class TrailSystem : public Component
{
public:
	~TrailSystem() override;
	TrailSystem() = default;

	void Start() override;
	void Update() override;

	TrailEmitter& AddEmitter(const TrailEmitterSettings& aSettings);

	std::vector<TrailEmitter>& GetEmitters() { return myEmitters; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	std::vector<TrailEmitter> myEmitters;
};

