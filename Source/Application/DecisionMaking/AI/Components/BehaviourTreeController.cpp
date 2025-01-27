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
#include "DecisionMaking/BrainTree.h"

class IsDead : public BrainTree::Decorator
{
public:
	Status update() override
	{
		if (myCurrentHealth <= 0.0f)
		{
			return Status::Success;
		}
		else
		{
			return Status::Failure;
		}
	}

private:
	float myCurrentHealth;
};

class IsLowHealth : public BrainTree::Decorator
{
public:
	Status update() override
	{
		if (myCurrentHealth / myMaxHealth > 0.5f)
		{
			return Status::Success;
		}
		else
		{
			return Status::Failure;
		}
	}

private:
	float myCurrentHealth;
	float myMaxHealth;
};

class WellInRange : public BrainTree::Decorator
{
public:
	Status update() override
	{
		return Status::Failure;
	}

private:
	CU::Vector3f myPosition;
	CU::Vector3f myWellPosition;
	float myWellRadius;
};

class EnemyInRange : public BrainTree::Decorator
{
public:
	Status update() override
	{
		return Status::Failure;
	}

private:
};

class EnemyInSightCone : public BrainTree::Decorator
{
public:
	Status update() override
	{
		return Status::Failure;
	}

private:
};

class EnemyInLOS : public BrainTree::Decorator
{
public:
	Status update() override
	{
		return Status::Failure;
	}

private:
};


class Die : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{

	}

	Status update() override
	{
		return Node::Status::Success;
	}

private:
};

class Idle : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	void initialize() override
	{

	}

	Status update() override
	{
		return Node::Status::Success;
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
		return Node::Status::Success;
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
		return Node::Status::Success;
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
		return Node::Status::Success;
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
		return Node::Status::Success;
	}

private:
};

void BehaviourTreeController::Start()
{
	BrainTree::Blackboard::Ptr blackboard = std::make_shared<BrainTree::Blackboard>();

	BrainTree::BehaviorTree tree = BrainTree::Builder()
		.composite<BrainTree::Selector>()
			.decorator<IsLowHealth>()
				.composite<BrainTree::Selector>()
					.decorator<IsDead>()
						.leaf<Die>(blackboard)
					.end()
					.decorator<WellInRange>()
						.leaf<Idle>(blackboard)
					.end()
					.leaf<SeekWell>(blackboard)
				.end()
			.end()
			.decorator<EnemyInRange>()
				.composite<BrainTree::Sequence>()
					.decorator<EnemyInSightCone>()
						.leaf<Aim>(blackboard)
					.end()
					.decorator<EnemyInLOS>()
						.leaf<Shoot>(blackboard)
					.end()
				.end()
			.end()
			.leaf<SeekEnemy>(blackboard)
		.end()
		.build();
}

void BehaviourTreeController::Update()
{
	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	myCurrentParticleActiveTime += dt;
	if (myIsShooting && myCurrentParticleActiveTime > myMaxParticleActiveTime)
	{
		gameObject->GetComponent<ParticleSystem>()->SetActive(false);
		myIsShooting = false;
	}
}

void BehaviourTreeController::SeekTarget()
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

bool BehaviourTreeController::IsLineOfSightClear(CU::Vector3f aOrigin, CU::Vector3f aDiff)
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
