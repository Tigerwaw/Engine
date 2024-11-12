#pragma once
#include "GameEngine/ComponentSystem/Component.h"

class WrapAroundWorld : public Component
{
public:
	void Start() override {}
	void Update() override;
};