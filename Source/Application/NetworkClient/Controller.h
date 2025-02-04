#pragma once
#include "GameEngine/ComponentSystem/Component.h"

class Controller : public Component
{
public:
	Controller(float aMoveSpeed, float aRotSpeed);

	void Start() override;
	void Update() override;
private:
	float myMoveSpeed = 50.0f;
	float myRotSpeed = 1.0f;
};

