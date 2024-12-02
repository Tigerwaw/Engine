#include "Enginepch.h"
#include "DecisionTreeController.h"
#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "../PollingStation.h"
#include "StateMachineController.h"

void DecisionTreeController::Start()
{
}

void DecisionTreeController::Update()
{
	if (!myTarget) return;

	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	auto transform = gameObject->GetComponent<Transform>();
	CU::Vector3f pos = transform->GetTranslation();

	CU::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
	CU::Vector3f directionToTarget = targetPos - pos;

	if (myHealth / myMaxHealth > 0.5f)
	{
		float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
		if (directionToTarget.LengthSqr() < myShootingRange * myShootingRange && dot >= mySightAngle)
		{
			myTimeSinceLastShot += dt;
			if (dot >= myShootingAngle && myTimeSinceLastShot > myShootingCooldown)
			{
				myTimeSinceLastShot = 0;
				myTarget->GetComponent<StateMachineController>()->TakeDamage(myDamage);
			}
			else
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
				transform->SetRotation(rot.GetEulerAnglesDegrees());
			}
		}
		else
		{
			myTarget = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("SMCont");
			SeekTarget();
		}
	}
	else
	{
		if (myHealth > 0)
		{
			if (directionToTarget.LengthSqr() < myHealRadius * myHealRadius)
			{
				Heal(myHPS * dt);
			}
			else
			{
				myTarget = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("HWell");
				SeekTarget();
			}
		}
		else
		{
			myCurrentDeathTime += dt;

			if (myCurrentDeathTime >= myDeathTimer)
			{
				myCurrentDeathTime = 0.0f;
				transform->SetTranslation(-500.0f, 0, 800.0f);
				Heal(myMaxHealth);
			}
		}
	}
}

void DecisionTreeController::TakeDamage(float aDamage)
{
	myHealth = std::max(myHealth - aDamage, 0.0f);
}

void DecisionTreeController::SeekTarget()
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
