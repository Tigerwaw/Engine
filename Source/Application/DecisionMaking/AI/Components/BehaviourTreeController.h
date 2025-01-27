#pragma once
#include "GameEngine/ComponentSystem/Component.h"

class BehaviourTreeController : public Component
{
public:
	enum class State
	{
		SeekEnemy,
		SeekWell,
		Aim,
		Heal,
		Death
	};

	void Start() override;
	void Update() override;
	void SetTarget(std::shared_ptr<GameObject> aGameObject) { myTarget = aGameObject; }

private:
	void SeekTarget();
	bool IsLineOfSightClear(CU::Vector3f aOrigin, CU::Vector3f aDirection);

	float myDamage = 25.0f;
	std::shared_ptr<GameObject> myTarget;

	float mySightAngle = 0.2f;
	float myShootingAngle = 0.6f;
	float myShootingRange = 600.0f;
	float myShootingCooldown = 2.0f;
	float myTimeSinceLastShot = 0.0f;

	float myHealRadius = 200.0f;
	float myHPS = 25.0f;

	float myDeathTimer = 2.0f;
	float myCurrentDeathTime = 0.0f;

	float myAvoidRadius = 200.0f;
	float myAvoidFactor = 6.0f;

	CU::Vector3f myVelocity;
	float myMaxAcceleration = 1.5f;
	float myDeceleration = 0.01f;
	float myMaxMoveSpeed = 50.0f;
	float myMinMoveSpeed = 5.0f;

	bool myOrientTowardsVelocity = true;
	float myCurrentRotationTime = 0;
	float myMaxRotationTime = 0.25f;
	CU::Quatf myCurrentRot;
	CU::Quatf myGoalRot;

	float myMaxParticleActiveTime = 0.5f;
	float myCurrentParticleActiveTime = 0;
	bool myIsShooting = false;
};

