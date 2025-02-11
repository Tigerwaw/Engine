#include "Enginepch.h"
#include "BehaviourTreeController.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "../PollingStation.h"
#include "DecisionMaking/HealthComponent.h"
#include "GameEngine/Intersections/Intersection3D.hpp"

class IsDead : public BrainTree::Decorator
{
public:
	IsDead(BrainTree::Blackboard::Ptr aBlackboard) : blackboard(aBlackboard) {}

	Status update() override
	{
		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto healthComp = self->GetComponent<HealthComponent>();
		if (healthComp->GetHealth() <= 0)
		{
			return child->tick();
		}

		return Status::Failure;
	}

private:
	BrainTree::Blackboard::Ptr blackboard;
};

class IsLowHealth : public BrainTree::Decorator
{
public:
	IsLowHealth(BrainTree::Blackboard::Ptr aBlackboard) : blackboard(aBlackboard) {}

	Status update() override
	{
		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		if (auto healthComp = self->GetComponent<HealthComponent>())
		{
			if (healthComp->GetHealth() / healthComp->GetMaxHealth() < 0.5f)
			{
				return child->tick();
			}
		}

		return Status::Failure;
	}

private:
	BrainTree::Blackboard::Ptr blackboard;
};

class WellInRange : public BrainTree::Decorator
{
public:
	WellInRange(BrainTree::Blackboard::Ptr aBlackboard) : blackboard(aBlackboard) {}

	Status update() override
	{
		float myHealRadius = blackboard->getFloat("myHealRadius");

		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto transform = self->GetComponent<Transform>();
		CU::Vector3f pos = transform->GetTranslation();

		auto well = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("HWell");
		CU::Vector3f targetPos = well->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f directionToTarget = targetPos - pos;
		if (directionToTarget.LengthSqr() < myHealRadius * myHealRadius)
		{
			return child->tick();
		}
		
		return Status::Failure;
	}

private:
	BrainTree::Blackboard::Ptr blackboard;
};

class EnemyInRange : public BrainTree::Decorator
{
public:
	EnemyInRange(BrainTree::Blackboard::Ptr aBlackboard) : blackboard(aBlackboard) {}

	Status update() override
	{
		float myShootingRange = blackboard->getFloat("myShootingRange");

		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto transform = self->GetComponent<Transform>();
		CU::Vector3f pos = transform->GetTranslation();

		auto target = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myTarget"));
		CU::Vector3f targetPos = target->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f directionToTarget = targetPos - pos;
		if (directionToTarget.LengthSqr() < myShootingRange * myShootingRange)
		{
			return child->tick();
		}

		return Status::Failure;
	}

private:
	BrainTree::Blackboard::Ptr blackboard;
};

class EnemyInLOS : public BrainTree::Decorator
{
public:
	EnemyInLOS(BrainTree::Blackboard::Ptr aBlackboard) : blackboard(aBlackboard) {}

	Status update() override
	{
		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto transform = self->GetComponent<Transform>();
		CU::Vector3f pos = transform->GetTranslation();

		auto target = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myTarget"));
		CU::Vector3f targetPos = target->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f directionToTarget = targetPos - pos;

		float mySightAngle = blackboard->getFloat("mySightAngle");
		float myAvoidRadius = blackboard->getFloat("myAvoidRadius");

		float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
		if (dot >= mySightAngle)
		{
			bool isLOSClear = true;

			for (auto& wallPos : PollingStation::Get().GetWallPositions())
			{
				if ((wallPos - pos).LengthSqr() > directionToTarget.LengthSqr()) continue;

				CU::Sphere<float> sphere(wallPos, myAvoidRadius);
				CU::Ray<float> ray(pos, directionToTarget.GetNormalized());

				if (CU::IntersectionSphereRay(sphere, ray))
				{
					return Status::Failure;
				}
			}

			if (isLOSClear)
			{
				return child->tick();
			}
		}
		
		return Status::Failure;
	}

private:
	BrainTree::Blackboard::Ptr blackboard;
};


class Death : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{

	}

	Status update() override
	{
		float myCurrentDeathTime = blackboard->getFloat("myCurrentDeathTime");
		float myDeathTimer = blackboard->getFloat("myDeathTimer");

		float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
		myCurrentDeathTime += dt;
		blackboard->setFloat("myCurrentDeathTime", myCurrentDeathTime);

		if (myCurrentDeathTime >= myDeathTimer)
		{
			myCurrentDeathTime = 0.0f;
			blackboard->setFloat("myCurrentDeathTime", myCurrentDeathTime);
			auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
			auto transform = self->GetComponent<Transform>();
			transform->SetTranslation(300.0f, 0, -500.0f);
			auto healthComp = self->GetComponent<HealthComponent>();
			healthComp->Heal(healthComp->GetMaxHealth());
			return Node::Status::Success;
		}
		
		return Node::Status::Running;
	}

private:
};

class Heal : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{

	}

	Status update() override
	{
		float myHPS = blackboard->getFloat("myHPS");

		float dt = Engine::GetInstance().GetTimer().GetDeltaTime();

		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto healthComp = self->GetComponent<HealthComponent>();
		healthComp->Heal(myHPS * dt);
		if (healthComp->GetHealth() > (healthComp->GetHealth() / healthComp->GetMaxHealth()) * 0.9f)
		{
			return Node::Status::Success;
		}

		return Node::Status::Running;
	}

private:
};

class SeekWell : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{

	}

	Status update() override
	{
		float myAvoidRadius = blackboard->getFloat("myAvoidRadius");
		float myAvoidFactor = blackboard->getFloat("myAvoidFactor");
		float myHealRadius = blackboard->getFloat("myHealRadius");
		float myMaxAcceleration = blackboard->getFloat("myMaxAcceleration");
		float myDeceleration = blackboard->getFloat("myDeceleration");
		CU::Vector3f myVelocity;
		myVelocity.x = blackboard->getFloat("myVelocityX");
		myVelocity.y = blackboard->getFloat("myVelocityY");
		myVelocity.z = blackboard->getFloat("myVelocityZ");
		CU::Quatf myCurrentRotation;
		myCurrentRotation.x = blackboard->getFloat("myCurrentRotationX");
		myCurrentRotation.y = blackboard->getFloat("myCurrentRotationY");
		myCurrentRotation.z = blackboard->getFloat("myCurrentRotationZ");
		myCurrentRotation.w = blackboard->getFloat("myCurrentRotationW");
		CU::Quatf myGoalRotation;
		myGoalRotation.x = blackboard->getFloat("myGoalRotationX");
		myGoalRotation.y = blackboard->getFloat("myGoalRotationY");
		myGoalRotation.z = blackboard->getFloat("myGoalRotationZ");
		myGoalRotation.w = blackboard->getFloat("myGoalRotationW");
		float myCurrentRotationTime = blackboard->getFloat("myCurrentRotationTime");
		float myMaxRotationTime = blackboard->getFloat("myMaxRotationTime");

		float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto& transform = self->GetComponent<Transform>();

		CU::Vector3f pos = transform->GetTranslation();
		CU::Vector3f targetPos = PollingStation::Get().GetHealingWellPosition();
		CU::Vector3f velocity;

		// Seek Target
		{
			velocity += targetPos - pos;
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
		blackboard->setFloat("myVelocityX", myVelocity.x);
		blackboard->setFloat("myVelocityY", myVelocity.y);
		blackboard->setFloat("myVelocityZ", myVelocity.z);
		transform->AddTranslation(myVelocity * dt);

		if (myVelocity.LengthSqr() > 1.0f)
		{
			myCurrentRotationTime += dt;
			blackboard->setFloat("myCurrentRotationTime", myCurrentRotationTime);
			if (myCurrentRotationTime >= myMaxRotationTime)
			{
				myCurrentRotationTime = 0;
				blackboard->setFloat("myCurrentRotationTime", myCurrentRotationTime);
				myCurrentRotation = myGoalRotation;
				blackboard->setFloat("myCurrentRotationX", myCurrentRotation.x);
				blackboard->setFloat("myCurrentRotationY", myCurrentRotation.y);
				blackboard->setFloat("myCurrentRotationZ", myCurrentRotation.z);
				blackboard->setFloat("myCurrentRotationW", myCurrentRotation.w);
				myGoalRotation = CU::Quatf(CU::Vector3f(0, std::atan2(myVelocity.x, myVelocity.z), 0));
				blackboard->setFloat("myGoalRotationX", myGoalRotation.x);
				blackboard->setFloat("myGoalRotationY", myGoalRotation.y);
				blackboard->setFloat("myGoalRotationZ", myGoalRotation.z);
				blackboard->setFloat("myGoalRotationW", myGoalRotation.w);
			}

			float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
			CU::Quatf rot = CU::Quatf::Slerp(myCurrentRotation, myGoalRotation, rotTimeDelta);
			self->GetComponent<Transform>()->SetRotation(rot.GetEulerAnglesDegrees());
		}

		myVelocity = myVelocity * (1 - myDeceleration);
		blackboard->setFloat("myVelocityX", myVelocity.x);
		blackboard->setFloat("myVelocityY", myVelocity.y);
		blackboard->setFloat("myVelocityZ", myVelocity.z);

		if ((targetPos - pos).LengthSqr() < myHealRadius * myHealRadius)
		{
			return Node::Status::Success;
		}

		return Node::Status::Failure;
	}

private:
};

class Aim : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{

	}

	Status update() override
	{
		float myCurrentRotationTime = blackboard->getFloat("myCurrentRotationTime");
		float myMaxRotationTime = blackboard->getFloat("myMaxRotationTime");
		CU::Quatf myCurrentRotation;
		myCurrentRotation.x = blackboard->getFloat("myCurrentRotationX");
		myCurrentRotation.y = blackboard->getFloat("myCurrentRotationY");
		myCurrentRotation.z = blackboard->getFloat("myCurrentRotationZ");
		myCurrentRotation.w = blackboard->getFloat("myCurrentRotationW");
		CU::Quatf myGoalRotation;
		myGoalRotation.x = blackboard->getFloat("myGoalRotationX");
		myGoalRotation.y = blackboard->getFloat("myGoalRotationY");
		myGoalRotation.z = blackboard->getFloat("myGoalRotationZ");
		myGoalRotation.w = blackboard->getFloat("myGoalRotationW");

		float myTimeSinceLastShot = blackboard->getFloat("myTimeSinceLastShot");
		float myShootingAngle = blackboard->getFloat("myShootingAngle");

		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto transform = self->GetComponent<Transform>();
		CU::Vector3f pos = transform->GetTranslation();

		auto target = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myTarget"));
		CU::Vector3f targetPos = target->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f directionToTarget = targetPos - pos;

		float dt = Engine::GetInstance().GetTimer().GetDeltaTime();

		myCurrentRotationTime += dt;
		blackboard->setFloat("myCurrentRotationTime", myCurrentRotationTime);
		if (myCurrentRotationTime >= myMaxRotationTime)
		{
			myCurrentRotationTime = 0;
			blackboard->setFloat("myCurrentRotationTime", myCurrentRotationTime);
			myCurrentRotation = myGoalRotation;
			blackboard->setFloat("myCurrentRotationX", myCurrentRotation.x);
			blackboard->setFloat("myCurrentRotationY", myCurrentRotation.y);
			blackboard->setFloat("myCurrentRotationZ", myCurrentRotation.z);
			blackboard->setFloat("myCurrentRotationW", myCurrentRotation.w);
			myGoalRotation = CU::Quatf(CU::Vector3f(0, std::atan2(directionToTarget.x, directionToTarget.z), 0));
			blackboard->setFloat("myGoalRotationX", myGoalRotation.x);
			blackboard->setFloat("myGoalRotationY", myGoalRotation.y);
			blackboard->setFloat("myGoalRotationZ", myGoalRotation.z);
			blackboard->setFloat("myGoalRotationW", myGoalRotation.w);
		}

		float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
		CU::Quatf rot = CU::Quatf::Slerp(myCurrentRotation, myGoalRotation, rotTimeDelta);
		transform->SetRotation(rot.GetEulerAnglesDegrees());
		float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
		myTimeSinceLastShot += dt;
		blackboard->setFloat("myTimeSinceLastShot", myTimeSinceLastShot);
		if (dot >= myShootingAngle)
		{
			return Node::Status::Success;
		}

		return Node::Status::Failure;
	}

private:
};

class Shoot : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{

	}

	Status update() override
	{
		float myTimeSinceLastShot = blackboard->getFloat("myTimeSinceLastShot");
		float myShootingCooldown = blackboard->getFloat("myShootingCooldown");
		float myDamage = blackboard->getFloat("myDamage");
		float myCurrentParticleActiveTime = blackboard->getFloat("myCurrentParticleActiveTime");

		if (myTimeSinceLastShot > myShootingCooldown)
		{
			myTimeSinceLastShot = 0;
			blackboard->setFloat("myTimeSinceLastShot", myTimeSinceLastShot);
			auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
			auto target = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myTarget"));
			auto healthComp = target->GetComponent<HealthComponent>();
			healthComp->TakeDamage(myDamage);

			if (healthComp->GetHealth() <= 0)
			{
				blackboard->setString("myTarget", PollingStation::Get().GetRandomAIActor(self)->GetName());
			}

			self->GetComponent<ParticleSystem>()->SetActive(true);
			myCurrentParticleActiveTime = 0;
			blackboard->setFloat("myCurrentParticleActiveTime", myCurrentParticleActiveTime);
			bool myIsShooting = true;
			blackboard->setBool("myIsShooting", myIsShooting);
			return Node::Status::Success;
		}

		return Node::Status::Failure;
	}

private:
};

class SeekEnemy : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{
	}

	Status update() override
	{
		float myAvoidRadius = blackboard->getFloat("myAvoidRadius");
		float myAvoidFactor = blackboard->getFloat("myAvoidFactor");
		float myMaxAcceleration = blackboard->getFloat("myMaxAcceleration");
		float myDeceleration = blackboard->getFloat("myDeceleration");
		CU::Vector3f myVelocity;
		myVelocity.x = blackboard->getFloat("myVelocityX");
		myVelocity.y = blackboard->getFloat("myVelocityY");
		myVelocity.z = blackboard->getFloat("myVelocityZ");
		CU::Quatf myCurrentRotation;
		myCurrentRotation.x = blackboard->getFloat("myCurrentRotationX");
		myCurrentRotation.y = blackboard->getFloat("myCurrentRotationY");
		myCurrentRotation.z = blackboard->getFloat("myCurrentRotationZ");
		myCurrentRotation.w = blackboard->getFloat("myCurrentRotationW");
		CU::Quatf myGoalRotation;
		myGoalRotation.x = blackboard->getFloat("myGoalRotationX");
		myGoalRotation.y = blackboard->getFloat("myGoalRotationY");
		myGoalRotation.z = blackboard->getFloat("myGoalRotationZ");
		myGoalRotation.w = blackboard->getFloat("myGoalRotationW");
		float myCurrentRotationTime = blackboard->getFloat("myCurrentRotationTime");
		float myMaxRotationTime = blackboard->getFloat("myMaxRotationTime");
		float myShootingRange = blackboard->getFloat("myShootingRange");

		float dt = Engine::GetInstance().GetTimer().GetDeltaTime();

		auto self = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myName"));
		auto target = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(blackboard->getString("myTarget"));
		auto& transform = self->GetComponent<Transform>();

		CU::Vector3f pos = transform->GetTranslation();
		CU::Vector3f targetPos = target->GetComponent<Transform>()->GetTranslation();
		CU::Vector3f velocity;

		if ((targetPos - self->GetComponent<Transform>()->GetTranslation()).LengthSqr() < myShootingRange * myShootingRange)
		{
			return Node::Status::Success;
		}

		// Seek Target
		{
			velocity += targetPos - pos;
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
		blackboard->setFloat("myVelocityX", myVelocity.x);
		blackboard->setFloat("myVelocityY", myVelocity.y);
		blackboard->setFloat("myVelocityZ", myVelocity.z);
		transform->AddTranslation(myVelocity * dt);

		if (myVelocity.LengthSqr() > 1.0f)
		{
			myCurrentRotationTime += dt;
			blackboard->setFloat("myCurrentRotationTime", myCurrentRotationTime);
			if (myCurrentRotationTime >= myMaxRotationTime)
			{
				myCurrentRotationTime = 0;
				blackboard->setFloat("myCurrentRotationTime", myCurrentRotationTime);
				myCurrentRotation = myGoalRotation;
				blackboard->setFloat("myCurrentRotationX", myCurrentRotation.x);
				blackboard->setFloat("myCurrentRotationY", myCurrentRotation.y);
				blackboard->setFloat("myCurrentRotationZ", myCurrentRotation.z);
				blackboard->setFloat("myCurrentRotationW", myCurrentRotation.w);
				myGoalRotation = CU::Quatf(CU::Vector3f(0, std::atan2(myVelocity.x, myVelocity.z), 0));
				blackboard->setFloat("myGoalRotationX", myGoalRotation.x);
				blackboard->setFloat("myGoalRotationY", myGoalRotation.y);
				blackboard->setFloat("myGoalRotationZ", myGoalRotation.z);
				blackboard->setFloat("myGoalRotationW", myGoalRotation.w);
			}

			float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
			CU::Quatf rot = CU::Quatf::Slerp(myCurrentRotation, myGoalRotation, rotTimeDelta);
			self->GetComponent<Transform>()->SetRotation(rot.GetEulerAnglesDegrees());
		}

		myVelocity = myVelocity * (1 - myDeceleration);
		blackboard->setFloat("myVelocityX", myVelocity.x);
		blackboard->setFloat("myVelocityY", myVelocity.y);
		blackboard->setFloat("myVelocityZ", myVelocity.z);

		return Node::Status::Failure;
	}

private:
};

void BehaviourTreeController::Start()
{
	myBlackboard = std::make_shared<BrainTree::Blackboard>();

	myBlackboard->setFloat("myDamage", myDamage);
	myBlackboard->setString("myName", gameObject->GetName());
	myBlackboard->setString("myTarget", "");
	myBlackboard->setFloat("mySightAngle", mySightAngle);
	myBlackboard->setFloat("myShootingAngle", myShootingAngle);
	myBlackboard->setFloat("myShootingRange", myShootingRange);
	myBlackboard->setFloat("myShootingCooldown", myShootingCooldown);
	myBlackboard->setFloat("myTimeSinceLastShot", myTimeSinceLastShot);
	myBlackboard->setFloat("myHealRadius", myHealRadius);
	myBlackboard->setFloat("myHPS", myHPS);
	myBlackboard->setFloat("myDeathTimer", myDeathTimer);
	myBlackboard->setFloat("myCurrentDeathTime", myCurrentDeathTime);
	myBlackboard->setFloat("myAvoidRadius", myAvoidRadius);
	myBlackboard->setFloat("myAvoidFactor", myAvoidFactor);
	myBlackboard->setFloat("myVelocityX", myVelocity.x);
	myBlackboard->setFloat("myVelocityY", myVelocity.y);
	myBlackboard->setFloat("myVelocityZ", myVelocity.z);
	myBlackboard->setFloat("myMaxAcceleration", myMaxAcceleration);
	myBlackboard->setFloat("myDeceleration", myDeceleration);
	myBlackboard->setFloat("myMaxMoveSpeed", myMaxMoveSpeed);
	myBlackboard->setFloat("myMinMoveSpeed", myMinMoveSpeed);
	myBlackboard->setBool("myOrientTowardsVelocity", myOrientTowardsVelocity);
	myBlackboard->setFloat("myCurrentRotationTime", myCurrentRotationTime);
	myBlackboard->setFloat("myMaxRotationTime", myMaxRotationTime);
	myBlackboard->setFloat("myCurrentRotX", myCurrentRot.x);
	myBlackboard->setFloat("myCurrentRotY", myCurrentRot.y);
	myBlackboard->setFloat("myCurrentRotZ", myCurrentRot.z);
	myBlackboard->setFloat("myCurrentRotW", myCurrentRot.w);
	myBlackboard->setFloat("myGoalRotX", myGoalRot.x);
	myBlackboard->setFloat("myGoalRotY", myGoalRot.y);
	myBlackboard->setFloat("myGoalRotZ", myGoalRot.z);
	myBlackboard->setFloat("myGoalRotW", myGoalRot.w);
	myBlackboard->setBool("myIsShooting", myIsShooting);
	myBlackboard->setFloat("myCurrentParticleActiveTime", myCurrentParticleActiveTime);

	myTree = BrainTree::Builder()
		.composite<BrainTree::Selector>()
			.decorator<IsLowHealth>(myBlackboard)
				.composite<BrainTree::Selector>()
					.decorator<IsDead>(myBlackboard)
						.leaf<Death>(myBlackboard)
					.end()
					.decorator<WellInRange>(myBlackboard)
						.leaf<Heal>(myBlackboard)
					.end()
					.leaf<SeekWell>(myBlackboard)
				.end()
			.end()
			.composite<BrainTree::Sequence>()
				.leaf<SeekEnemy>(myBlackboard)
				.decorator<EnemyInRange>(myBlackboard)
					.leaf<Aim>(myBlackboard)
				.end()
				.decorator<EnemyInLOS>(myBlackboard)
					.leaf<Shoot>(myBlackboard)
				.end()
			.end()
		.end()
	.build();
}

void BehaviourTreeController::Update()
{
	myTree.update();

	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	myCurrentParticleActiveTime = myBlackboard->getFloat("myCurrentParticleActiveTime");
	myCurrentParticleActiveTime += dt;
	myBlackboard->setFloat("myCurrentParticleActiveTime", myCurrentParticleActiveTime);
	myIsShooting = myBlackboard->getBool("myIsShooting");
	if (myIsShooting && myCurrentParticleActiveTime > myMaxParticleActiveTime)
	{
		gameObject->GetComponent<ParticleSystem>()->SetActive(false);
		myIsShooting = false;
		myBlackboard->setBool("myIsShooting", myIsShooting);
	}
}

void BehaviourTreeController::SetTarget(std::shared_ptr<GameObject> aGameObject)
{
	myTarget = aGameObject;
	myBlackboard->setString("myTarget", myTarget->GetName());
}
