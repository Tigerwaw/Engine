#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Vector.hpp"



class ObjectController : public Component
{
public:
	ObjectController(float aMoveSpeed, float aRotSpeed);

	void Start() override;
	void Update() override;
private:
	float myMoveSpeed = 0;
	float myMoveSpeedMultiplier = 1;
	float myRotSpeed;
};

