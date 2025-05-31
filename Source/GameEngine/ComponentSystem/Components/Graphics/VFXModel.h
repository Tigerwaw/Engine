#pragma once
#include "ComponentSystem/Component.h"
#include "ComponentSystem/GameObjectEvent.h"
#include "Math/Vector.hpp"


class VFXModel : public Component
{
public:
	struct VFXData
	{
		float lifetime = 5.0f;
		GameObjectEventType eventTrigger = GameObjectEventType::None;
		bool isBillboard = false;
		bool isSpritesheet = false;
		Math::Vector2f spritesheetSize = { 0, 0 };
	};

	~VFXModel() override;
	VFXModel() = default;
	VFXModel(VFXData aVFXData);

	void Start() override;
	void Update() override;

	void PlayVFX();
	void ReceiveEvent(const GameObjectEvent& aEvent) override;

	void SetLifetime(float aLifetime) { myLifetime = aLifetime; }
	void SetEventTrigger(GameObjectEventType aEventTrigger) { myEventTrigger = aEventTrigger; }
	void SetWorldspaceSprite(bool aIsBillboard = false, bool aIsSpritesheet = false, Math::Vector2f aSpritesheetSize = Math::Vector2f());
	void SetCustomShaderParams(Math::Vector4<float> aParams) { myCustomShaderParameters = aParams; }

	void SetCastShadows(bool aCastShadows) { myCastShadows = aCastShadows; }
	const bool GetCastShadows() const { return myCastShadows; }

	Math::Vector4<float> GetCustomShaderParams() const { return myCustomShaderParameters; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
protected:

	float myCurrentTimeAlive = 0;
	float myLifetime = 1.0f;
	GameObjectEventType myEventTrigger = GameObjectEventType::None;
	bool myIsBillboard = false;
	Math::Vector4<float> myCustomShaderParameters;
	bool myCastShadows = true;
};

