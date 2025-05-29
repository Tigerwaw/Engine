#include "Enginepch.h"

#include "ObjectController.h"
#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

ObjectController::ObjectController(float aMoveSpeed, float aRotSpeed)
{
	myMoveSpeed = aMoveSpeed;
	myMoveSpeedMultiplier = 1.0f;
	myRotSpeed = aRotSpeed;
}

void ObjectController::Start()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.RegisterBinaryAction("ObjectSpeedUp", Keys::SHIFT, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("ObjectSpeedDown", Keys::SHIFT, GenericInput::ActionType::Released);
	inputHandler.RegisterBinaryAction("ObjectReset", Keys::R, GenericInput::ActionType::Clicked);
	inputHandler.RegisterAnalogAction("ObjectXMovement", Keys::A, Keys::D);
	inputHandler.RegisterAnalogAction("ObjectYMovement", Keys::CONTROL, Keys::SPACE);
	inputHandler.RegisterAnalogAction("ObjectZMovement", Keys::S, Keys::W);
	inputHandler.RegisterAnalogAction("ObjectYRotation", Keys::Q, Keys::E);
}

void ObjectController::Update()
{
	float deltaTime = Engine::Get().GetTimer().GetDeltaTime();
	InputHandler& inputHandler = Engine::Get().GetInputHandler();

	if (inputHandler.GetBinaryAction("ObjectSpeedUp"))
	{
		myMoveSpeedMultiplier = 2.0f;
	}
	else if (inputHandler.GetBinaryAction("ObjectSpeedDown"))
	{
		myMoveSpeedMultiplier = 1.0f;
	}

	if (inputHandler.GetBinaryAction("ObjectReset"))
	{
		gameObject->GetComponent<Transform>()->SetRotation(0, 0, 0);
		gameObject->GetComponent<Transform>()->SetTranslation(0, 0, 0);
	}

	CU::Vector3f inputDelta;

	inputDelta += gameObject->GetComponent<Transform>()->GetRightVector() * inputHandler.GetAnalogAction("ObjectXMovement");
	inputDelta += gameObject->GetComponent<Transform>()->GetUpVector() * inputHandler.GetAnalogAction("ObjectYMovement");
	inputDelta += gameObject->GetComponent<Transform>()->GetForwardVector() * inputHandler.GetAnalogAction("ObjectZMovement");

	if (inputDelta.LengthSqr() > 1.0f)
	{
		inputDelta.Normalize();
	}


	CU::Vector3f rotationDelta;
	rotationDelta.y = inputHandler.GetAnalogAction("ObjectYRotation");

	rotationDelta *= myRotSpeed * deltaTime;

	gameObject->GetComponent<Transform>()->AddRotation(rotationDelta);
	gameObject->GetComponent<Transform>()->SetTranslation(gameObject->GetComponent<Transform>()->GetTranslation() + inputDelta * myMoveSpeed * myMoveSpeedMultiplier * deltaTime);
}
