#include "Enginepch.h"
#include "ControllerMoveWeighted.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

#include "Movement2/AI/Controllers/ControllerBase.h"
#include "Movement2/AI/Controllers/Separate.h"
#include "Movement2/AI/Controllers/Cohesion.h"
#include "Movement2/AI/Controllers/VelocityMatch.h"
#include "Movement2/AI/Controllers/Wander.h"
#include "Movement2/AI/Controllers/CollisionAvoidance.h"

ControllerMoveWeighted::~ControllerMoveWeighted()
{
	for (auto& [type, cont] : mySteeringBehaviours)
	{
		delete cont;
	}
}

ControllerMoveWeighted::ControllerMoveWeighted(float aMaxMoveSpeed, float aMaxAcceleration)
{
	SetMaxMoveSpeed(aMaxMoveSpeed);
	SetMaxAcceleration(aMaxAcceleration);
}

void ControllerMoveWeighted::SetMaxMoveSpeed(float aMaxMoveSpeed)
{
	myMaxMoveSpeed = aMaxMoveSpeed;
}

void ControllerMoveWeighted::SetMaxAcceleration(float aMaxAcceleration)
{
	myMaxAcceleration = aMaxAcceleration;
}

void ControllerMoveWeighted::AddControllerType(ControllerType aControllerType, float aBlendWeight)
{
	if (mySteeringBehaviours.contains(aControllerType)) return;

	switch (aControllerType)
	{
	case ControllerMoveWeighted::ControllerType::Separate:
		mySteeringBehaviours[aControllerType] = new Separate();
		break;
	case ControllerMoveWeighted::ControllerType::Cohesion:
		mySteeringBehaviours[aControllerType] = new Cohesion();
		break;
	case ControllerMoveWeighted::ControllerType::VelocityMatch:
		mySteeringBehaviours[aControllerType] = new VelocityMatch();
		break;
	case ControllerMoveWeighted::ControllerType::Wander:
		mySteeringBehaviours[aControllerType] = new Wander();
		break;
	case ControllerMoveWeighted::ControllerType::CollisionAvoidance:
		mySteeringBehaviours[aControllerType] = new CollisionAvoidance();
		break;
	default:
		break;
	}

	mySteeringWeights[aControllerType] = aBlendWeight;
}

void ControllerMoveWeighted::RemoveControllerType(ControllerType aControllerType)
{
	if (!mySteeringBehaviours.contains(aControllerType)) return;

	delete mySteeringBehaviours[aControllerType];
	mySteeringBehaviours.erase(aControllerType);
	mySteeringWeights.erase(aControllerType);
}

void ControllerMoveWeighted::Start()
{
}

void ControllerMoveWeighted::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();
	auto& transform = gameObject->GetComponent<Transform>();

	ControllerBase::SteeringInput steeringInput;
	steeringInput.velocity = myVelocity;
	steeringInput.position = transform->GetTranslation();
	steeringInput.orientation = transform->GetForwardVector();

	ControllerBase::SteeringOutput steering;
	
	for (auto& [type, controller] : mySteeringBehaviours)
	{
		steering += controller->GetSteering(steeringInput) * mySteeringWeights[type];
	}

	// Clamp steering velocity to max acceleration
	if (steering.velocity.LengthSqr() > myMaxAcceleration * myMaxAcceleration)
	{
		steering.velocity.Normalize();
		steering.velocity *= myMaxAcceleration;
	}

	myVelocity += steering.velocity;

	// Clamp velocity min speed
	if (myVelocity.LengthSqr() < myMinMoveSpeed * myMinMoveSpeed)
	{
		CU::Vector3f randomVelocity;
		randomVelocity.x = static_cast<float>((std::rand() % 100) - (std::rand() % 100));
		randomVelocity.z = static_cast<float>((std::rand() % 100) - (std::rand() % 100));
		myVelocity += randomVelocity.GetNormalized() * myMinMoveSpeed;
	}

	// Clamp velocity to max speed
	if (myVelocity.LengthSqr() > myMaxMoveSpeed * myMaxMoveSpeed)
	{
		myVelocity.Normalize();
		myVelocity *= myMaxMoveSpeed;
	}

	transform->AddTranslation(myVelocity * dt);
	
	if (myOrientTowardsVelocity)
	{
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
	}
	else
	{
		transform->AddRotation(0, steering.rotation * dt, 0);
	}

	myVelocity = myVelocity * (1 - myDeceleration);
}
