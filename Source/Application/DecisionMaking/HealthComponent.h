#pragma once
#include "GameEngine/ComponentSystem/Component.h"

class HealthComponent : public Component
{
public:
	void Start() override;
	void Update() override;
	void TakeDamage(float aDamage);
	void Heal(float aHP);

	const float GetHealth() const { return myHealth; }
	const float GetMaxHealth() const { return myMaxHealth; }
protected:
	void UpdateMaterial();

	float myHealth;
	float myMaxHealth;
};

