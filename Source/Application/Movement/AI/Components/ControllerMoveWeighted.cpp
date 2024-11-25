#include "Enginepch.h"
#include "ControllerMoveWeighted.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

#include "Movement/AI/Controllers/ControllerBase.h"
#include "Movement/AI/Controllers/Separate.h"
#include "Movement/AI/Controllers/Cohesion.h"
#include "Movement/AI/Controllers/VelocityMatch.h"
#include "Movement/AI/Controllers/Wander.h"

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
	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	auto& transform = gameObject->GetComponent<Transform>();

	ControllerBase::SteeringInput steeringInput;
	steeringInput.velocity = myVelocity;
	steeringInput.position = transform->GetTranslation();
	steeringInput.orientation = transform->GetForwardVector();

	if (myTarget)
	{
		if (auto& cont = myTarget->GetComponent<ControllerMoveWeighted>())
		{
			steeringInput.targetVelocity = cont->GetVelocity();
		}
	}

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

	// Clamp velocity to max speed
	if (myVelocity.LengthSqr() > myMaxMoveSpeed * myMaxMoveSpeed)
	{
		myVelocity.Normalize();
		myVelocity *= myMaxMoveSpeed;
	}

	transform->AddTranslation(myVelocity * dt);
	transform->AddRotation(0, steering.rotation * dt, 0);
	myVelocity = myVelocity * (1 - myDeceleration);
}
