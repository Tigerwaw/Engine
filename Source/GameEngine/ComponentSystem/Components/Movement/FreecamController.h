#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector.hpp"

namespace CU = CommonUtilities;

class FreecamController : public Component
{
public:
	FreecamController() {}
	FreecamController(float aMoveSpeed, float aRotSpeed);

	void Start() override;
	void Update() override;

	void SetMoveSpeed(float aMoveSpeed);
	void SetRotationSpeed(float aRotSpeed);

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	float myMoveSpeed = 0;
	float myMoveSpeedMultiplier = 1.0f;
	CU::Vector2f myRotSpeed;
};

