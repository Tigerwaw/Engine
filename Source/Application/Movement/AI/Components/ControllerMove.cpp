#include "Enginepch.h"
#include "ControllerMove.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

#include "Movement/AI/Controllers/ControllerBase.h"
#include "Movement/AI/Controllers/Wander.h"
#include "Movement/AI/Controllers/Seek.h"
#include "Movement/AI/Controllers/Separate.h"

ControllerMove::~ControllerMove()
{
	delete myController;
}

ControllerMove::ControllerMove(float aMaxMoveSpeed, float aMaxAcceleration, ControllerType aControllerType)
{
	SetMaxMoveSpeed(aMaxMoveSpeed);
	SetMaxAcceleration(aMaxAcceleration);
	SetControllerType(aControllerType);
}

void ControllerMove::SetMaxMoveSpeed(float aMaxMoveSpeed)
{
	myMaxMoveSpeed = aMaxMoveSpeed;
}

void ControllerMove::SetMaxAcceleration(float aMaxAcceleration)
{
	myMaxAcceleration = aMaxAcceleration;
}

void ControllerMove::SetControllerType(ControllerType aControllerType)
{
	switch (aControllerType)
	{
	case ControllerMove::ControllerType::Wander:
		myController = new Wander();
		break;
	case ControllerMove::ControllerType::Seek:
		myController = new Seek();
		break;
	case ControllerMove::ControllerType::Separate:
		myController = new Separate();
		break;
	default:
		break;
	}

	if (myController)
	{
		myController->Start();
	}
}

void ControllerMove::Start()
{
}

void ControllerMove::Update()
{
	float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
	auto& transform = gameObject->GetComponent<Transform>();

	ControllerBase::SteeringInput steeringInput;
	steeringInput.position = transform->GetTranslation();
	steeringInput.orientation = transform->GetForwardVector();
	ControllerBase::SteeringOutput steering = myController->GetSteering(steeringInput);

	// Clamp steering velocity to max acceleration
	if (steering.velocity.LengthSqr() > myMaxAcceleration * myMaxAcceleration)
	{
		steering.velocity.Normalize();
		steering.velocity *= myMaxAcceleration;
	}

	myVelocity += steering.velocity;

	//if (myVelocity.LengthSqr() < 0.1f)
	//{
	//	myVelocity = CU::Vector3f(0, 0, 0);
	//}

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
