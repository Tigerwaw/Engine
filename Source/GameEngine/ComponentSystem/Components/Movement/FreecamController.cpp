#include "Enginepch.h"

#include "FreecamController.h"
#include "Engine.h"
#include "Time/Timer.h"
#include "Input/InputHandler.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"

FreecamController::FreecamController(float aMoveSpeed, float aRotSpeed)
{
	SetMoveSpeed(aMoveSpeed);
	SetRotationSpeed(aRotSpeed);
}

void FreecamController::Start()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.RegisterBinaryAction("CameraActivate", Keys::MOUSERBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("CameraActivate", ControllerButtons::LEFT_SHOULDER, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("CameraSpeedUp", Keys::SHIFT, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("CameraSpeedUp", ControllerButtons::RIGHT_SHOULDER, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("CameraSpeedDown", Keys::SHIFT, GenericInput::ActionType::Released);
	inputHandler.RegisterBinaryAction("CameraSpeedDown", ControllerButtons::RIGHT_SHOULDER, GenericInput::ActionType::Released);
	inputHandler.RegisterBinaryAction("CameraReset", Keys::R, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("CameraReset", ControllerButtons::BACK, GenericInput::ActionType::Clicked);
	inputHandler.RegisterAnalogAction("CameraXMovement", Keys::A, Keys::D);
	inputHandler.RegisterAnalogAction("CameraXMovement", ControllerAnalog::LEFT_STICK_X);
	inputHandler.RegisterAnalogAction("CameraYMovement", Keys::CONTROL, Keys::SPACE);
	inputHandler.RegisterAnalogAction("CameraYMovement", ControllerButtons::B, ControllerButtons::A);
	inputHandler.RegisterAnalogAction("CameraZMovement", Keys::S, Keys::W);
	inputHandler.RegisterAnalogAction("CameraZMovement", ControllerAnalog::LEFT_STICK_Y);
	inputHandler.RegisterAnalog2DAction("CameraRotation", MouseMovement2D::MousePosDelta);
	inputHandler.RegisterAnalog2DAction("CameraRotation", ControllerAnalog2D::RIGHT_STICK);
}

void FreecamController::Update()
{
	float deltaTime = Engine::Get().GetTimer().GetDeltaTime();
	InputHandler& inputHandler = Engine::Get().GetInputHandler();

	if (!inputHandler.GetBinaryAction("CameraActivate"))
	{
		return;
	}

	if (inputHandler.GetBinaryAction("CameraSpeedUp"))
	{
		myMoveSpeedMultiplier = 2.0f;
	}
	else if (inputHandler.GetBinaryAction("CameraSpeedDown"))
	{
		myMoveSpeedMultiplier = 1.0f;
	}

	if (inputHandler.GetBinaryAction("CameraReset"))
	{
		gameObject->GetComponent<Transform>()->SetRotation(0, 0, 0);
		gameObject->GetComponent<Transform>()->SetTranslation(0, 0, 0);
	}

	Math::Vector3f inputDelta;

	inputDelta += gameObject->GetComponent<Transform>()->GetRightVector() * inputHandler.GetAnalogAction("CameraXMovement");
	inputDelta += gameObject->GetComponent<Transform>()->GetUpVector() * inputHandler.GetAnalogAction("CameraYMovement");
	inputDelta += gameObject->GetComponent<Transform>()->GetForwardVector() * inputHandler.GetAnalogAction("CameraZMovement");

	if (inputDelta.LengthSqr() > 1.0f)
	{
		inputDelta.Normalize();
	}


	Math::Vector2f rotInput = inputHandler.GetAnalogAction2D("CameraRotation");
	Math::Vector3f rotationDelta;
	rotationDelta.x = -rotInput.y * myRotSpeed.y * deltaTime;
	rotationDelta.y = rotInput.x * myRotSpeed.x * deltaTime;

	gameObject->GetComponent<Transform>()->AddRotation(rotationDelta);
	gameObject->GetComponent<Transform>()->SetTranslation(gameObject->GetComponent<Transform>()->GetTranslation() + inputDelta * myMoveSpeed * myMoveSpeedMultiplier * deltaTime);
}

void FreecamController::SetMoveSpeed(float aMoveSpeed)
{
	myMoveSpeed = aMoveSpeed;
}

void FreecamController::SetRotationSpeed(float aRotSpeed)
{
	myRotSpeed = { aRotSpeed, aRotSpeed * 0.5f };
}

bool FreecamController::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool FreecamController::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("MoveSpeed"))
	{
		SetMoveSpeed(aJsonObject["MoveSpeed"].get<float>());
	}

	if (aJsonObject.contains("RotationSpeed"))
	{
		SetRotationSpeed(aJsonObject["RotationSpeed"].get<float>());
	}

	return true;
}
