#include "Enginepch.h"
#include "ControllerMove.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

#include "Movement1/AI/Controllers/ControllerBase.h"
#include "Movement1/AI/Controllers/Wander.h"
#include "Movement1/AI/Controllers/Seek.h"
#include "Movement1/AI/Controllers/Separate.h"

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
		myOrientTowardsVelocity = false;
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
}

void ControllerMove::Start()
{
}

void ControllerMove::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();
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
				myGoalRot = Math::Quatf(Math::Vector3f(0, std::atan2(myVelocity.x, myVelocity.z), 0));
			}

			float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
			Math::Quatf rot = Math::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
			gameObject->GetComponent<Transform>()->SetRotation(rot.GetEulerAnglesDegrees());
		}
	}
	else
	{
		transform->AddRotation(0, steering.rotation * dt, 0);
	}

	myVelocity = myVelocity * (1 - myDeceleration);
}
