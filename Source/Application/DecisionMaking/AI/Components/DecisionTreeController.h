#pragma once
#include "GameEngine/ComponentSystem/Component.h"

class DecisionTreeController : public Component
{
public:
	void Start() override;
	void Update() override;

	void SetTarget(std::shared_ptr<GameObject> aGameObject) { myTarget = aGameObject; }
	void Heal(float aHP) { myHealth = min(myHealth + aHP, myMaxHealth); }
	void TakeDamage(float aDamage);

private:
	void SeekTarget();

	float myHealth = 100.0f;
	float myMaxHealth = 100.0f;
	float myDamage = 30.0f;
	std::shared_ptr<GameObject> myTarget;

	float mySightAngle = 0.2f;
	float myShootingAngle = 0.1f;
	float myShootingRange = 600.0f;
	float myShootingCooldown = 2.0f;
	float myTimeSinceLastShot = 0.0f;

	float myHealRadius = 200.0f;
	float myHPS = 1.0f;

	float myDeathTimer = 2.0f;
	float myCurrentDeathTime = 0.0f;

	float myAvoidRadius = 200.0f;
	float myAvoidFactor = 10.0f;

	CU::Vector3f myVelocity;
	float myMaxAcceleration = 1.0f;
	float myDeceleration = 0.01f;
	float myMaxMoveSpeed = 50.0f;
	float myMinMoveSpeed = 5.0f;

	bool myOrientTowardsVelocity = true;
	float myCurrentRotationTime = 0;
	float myMaxRotationTime = 0.25f;
	CU::Quatf myCurrentRot;
	CU::Quatf myGoalRot;
};

