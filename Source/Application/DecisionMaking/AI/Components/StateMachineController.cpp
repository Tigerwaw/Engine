#include "Enginepch.h"
#include "StateMachineController.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "../PollingStation.h"
#include "DecisionTreeController.h"

void StateMachineController::Start()
{
}

void StateMachineController::Update()
{
	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	myCurrentParticleActiveTime += dt;
	if (myIsShooting && myCurrentParticleActiveTime > myMaxParticleActiveTime)
	{
		gameObject->GetComponent<ParticleSystem>()->SetActive(false);
		myIsShooting = false;
	}

	switch (myCurrentState)
	{
	case StateMachineController::State::SeekEnemy:
	{
		myTarget = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("DTCont");
		SeekTarget();

		auto transform = gameObject->GetComponent<Transform>();
		CU::Vector3f pos = transform->GetTranslation();
		CU::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f directionToTarget = targetPos - pos;
		if (directionToTarget.LengthSqr() < myShootingRange * myShootingRange)
		{
			float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
			if (dot >= mySightAngle)
			{
				myCurrentState = State::Aim;
			}
		}

		break;
	}
	case StateMachineController::State::SeekWell:
	{
		myTarget = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("HWell");
		SeekTarget();

		auto transform = gameObject->GetComponent<Transform>();
		CU::Vector3f pos = transform->GetTranslation();
		CU::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f directionToTarget = targetPos - pos;
		if (directionToTarget.LengthSqr() >= myHealRadius * myHealRadius)
		{
			myCurrentState = State::Heal;
		}

		break;
	}
	case StateMachineController::State::Aim:
	{
		myCurrentRotationTime += dt;
		if (myCurrentRotationTime >= myMaxRotationTime)
		{
			myCurrentRotationTime = 0;
			myCurrentRot = myGoalRot;
			myGoalRot = CU::Quatf(CU::Vector3f(0, std::atan2(myVelocity.x, myVelocity.z), 0));
		}

		float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
		CU::Quatf rot = CU::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
		auto transform = gameObject->GetComponent<Transform>();
		transform->SetRotation(rot.GetEulerAnglesDegrees());

		CU::Vector3f pos = transform->GetTranslation();
		CU::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f directionToTarget = pos - targetPos;
		float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
		myTimeSinceLastShot += dt;
		if (dot >= myShootingAngle)
		{
			if (myTimeSinceLastShot > myShootingCooldown)
			{
				myTimeSinceLastShot = 0;
				myTarget->GetComponent<DecisionTreeController>()->TakeDamage(myDamage);
				gameObject->GetComponent<ParticleSystem>()->SetActive(true);
				myCurrentParticleActiveTime = 0;
				myIsShooting = true;
			}
		}
		else if (dot < mySightAngle)
		{
			myCurrentState = State::SeekEnemy;
		}
		break;
	}
	case StateMachineController::State::Heal:
	{
		Heal(myHPS * dt);
		if (myHealth / myMaxHealth > 0.9f)
		{
			myCurrentState = State::SeekEnemy;
		}
		break;
	}
	case StateMachineController::State::Death:
	{
		myCurrentDeathTime += dt;

		if (myCurrentDeathTime >= myDeathTimer)
		{
			myCurrentDeathTime = 0.0f;
			auto transform = gameObject->GetComponent<Transform>();
			transform->SetTranslation(-500.0f, 0, 800.0f);
			Heal(myMaxHealth);
			myCurrentState = State::SeekEnemy;
		}
		break;
	}
	default:
		break;
	}
}

void StateMachineController::TakeDamage(float aDamage)
{
	myHealth = std::max(myHealth - aDamage, 0.0f);

	if (myHealth <= 0)
	{
		myCurrentState = State::Death;
	}
}

void StateMachineController::SeekTarget()
{
	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	auto& transform = gameObject->GetComponent<Transform>();

	CU::Vector3f pos = transform->GetTranslation();
	CU::Vector3f velocity;

	// Seek Target
	{
		velocity += myTarget->GetComponent<Transform>()->GetTranslation() - pos;
	}

	// Avoid walls
	{
		CU::Vector3f avoidVelocity;
		for (auto& wallPos : PollingStation::Get().GetWallPositions())
		{
			CU::Vector3f diff = pos - wallPos;
			if (diff.LengthSqr() > myAvoidRadius * myAvoidRadius) continue;
			avoidVelocity += diff;
		}

		avoidVelocity *= myAvoidFactor;
		velocity += avoidVelocity;
	}

	// Clamp steering velocity to max acceleration
	if (velocity.LengthSqr() > myMaxAcceleration * myMaxAcceleration)
	{
		velocity.Normalize();
		velocity *= myMaxAcceleration;
	}

	myVelocity += velocity;
	transform->AddTranslation(myVelocity * dt);

	if (myVelocity.LengthSqr() > 1.0f)
	{
		myCurrentRotationTime += dt;
		if (myCurrentRotationTime >= myMaxRotationTime)
		{
			myCurrentRotationTime = 0;
			myCurrentRot = myGoalRot;
			myGoalRot = CU::Quatf(CU::Vector3f(0, std::atan2(myVelocity.x, myVelocity.z), 0));
		}

		float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
		CU::Quatf rot = CU::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
		gameObject->GetComponent<Transform>()->SetRotation(rot.GetEulerAnglesDegrees());
	}

	myVelocity = myVelocity * (1 - myDeceleration);
}
