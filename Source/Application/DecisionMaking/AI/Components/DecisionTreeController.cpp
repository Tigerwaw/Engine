#include "Enginepch.h"
#include "DecisionTreeController.h"
#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "../PollingStation.h"
#include "DecisionMaking/HealthComponent.h"
#include "GameEngine/Intersections/Intersection3D.hpp"

void DecisionTreeController::Start()
{
}

void DecisionTreeController::Update()
{
	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	myCurrentParticleActiveTime += dt;
	if (myIsShooting && myCurrentParticleActiveTime > myMaxParticleActiveTime)
	{
		gameObject->GetComponent<ParticleSystem>()->SetActive(false);
		myIsShooting = false;
	}

	if (!myTarget) return;

	auto transform = gameObject->GetComponent<Transform>();
	CU::Vector3f pos = transform->GetTranslation();

	CU::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
	CU::Vector3f directionToTarget = targetPos - pos;

	auto healthComp = gameObject->GetComponent<HealthComponent>();

	if (healthComp->GetHealth() / healthComp->GetMaxHealth() > 0.5f)
	{
		SetTarget(PollingStation::Get().GetRandomAIActor(Engine::GetInstance().GetSceneHandler().FindGameObjectByName(gameObject->GetName())));
		float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
		if (directionToTarget.LengthSqr() < myShootingRange * myShootingRange && dot >= mySightAngle && IsLineOfSightClear(pos, directionToTarget))
		{
			myTimeSinceLastShot += dt;
			if (dot >= myShootingAngle && myTimeSinceLastShot > myShootingCooldown)
			{
				myTimeSinceLastShot = 0;
				auto targetHealthComp = myTarget->GetComponent<HealthComponent>();
				targetHealthComp->TakeDamage(myDamage);

				if (targetHealthComp->GetHealth() <= 0)
				{
					SetTarget(PollingStation::Get().GetRandomAIActor(Engine::GetInstance().GetSceneHandler().FindGameObjectByName(gameObject->GetName())));
				}

				gameObject->GetComponent<ParticleSystem>()->SetActive(true);
				myCurrentParticleActiveTime = 0;
				myIsShooting = true;
			}
			else
			{
				myCurrentRotationTime += dt;
				if (myCurrentRotationTime >= myMaxRotationTime)
				{
					myCurrentRotationTime = 0;
					myCurrentRot = myGoalRot;
					myGoalRot = CU::Quatf(CU::Vector3f(0, std::atan2(directionToTarget.x, directionToTarget.z), 0));
				}

				float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
				CU::Quatf rot = CU::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
				transform->SetRotation(rot.GetEulerAnglesDegrees());
			}
		}
		else
		{
			SeekTarget();
		}
	}
	else
	{
		if (healthComp->GetHealth() > 0)
		{
			SetTarget(Engine::GetInstance().GetSceneHandler().FindGameObjectByName("HWell"));
			if (directionToTarget.LengthSqr() < myHealRadius * myHealRadius)
			{
				healthComp->Heal(myHPS * dt);
			}
			else
			{
				SeekTarget();
			}
		}
		else
		{
			myCurrentDeathTime += dt;

			if (myCurrentDeathTime >= myDeathTimer)
			{
				myCurrentDeathTime = 0.0f;
				transform->SetTranslation(-200.0f, 0, 500.0f);
				healthComp->Heal(healthComp->GetMaxHealth());
				myTarget = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("SMCont");
			}
		}
	}
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

bool DecisionTreeController::IsLineOfSightClear(CU::Vector3f aOrigin, CU::Vector3f aDiff)
{
	for (auto& wallPos : PollingStation::Get().GetWallPositions())
	{
		if ((wallPos - aOrigin).LengthSqr() > aDiff.LengthSqr()) continue;

		CU::Sphere<float> sphere(wallPos, myAvoidRadius);
		CU::Ray<float> ray(aOrigin, aDiff.GetNormalized());

		if (CU::IntersectionSphereRay(sphere, ray))
		{
			return false;
		}
	}

	return true;
}