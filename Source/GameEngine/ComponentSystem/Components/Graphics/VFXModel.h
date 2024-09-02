#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/ComponentSystem/GameObjectEvent.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class VFXModel : public Component
{
public:
	struct VFXData
	{
		float lifetime = 5.0f;
		GameObjectEventType eventTrigger = GameObjectEventType::None;
		bool isBillboard = false;
		bool isSpritesheet = false;
		CU::Vector2<float> spritesheetSize = { 0, 0 };
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
	void SetWorldspaceSprite(bool aIsBillboard = false, bool aIsSpritesheet = false, CU::Vector2f aSpritesheetSize = CU::Vector2f());
	void SetCustomShaderParams(CU::Vector4<float> aParams) { myCustomShaderParameters = aParams; }

	void SetCastShadows(bool aCastShadows) { myCastShadows = aCastShadows; }
	const bool GetCastShadows() const { return myCastShadows; }

	CU::Vector4<float> GetCustomShaderParams() const { return myCustomShaderParameters; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
protected:

	float myCurrentTimeAlive = 0;
	float myLifetime = 1.0f;
	GameObjectEventType myEventTrigger = GameObjectEventType::None;
	bool myIsBillboard = false;
	CU::Vector4<float> myCustomShaderParameters;
	bool myCastShadows = true;
};

