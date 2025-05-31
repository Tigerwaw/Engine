#include "Enginepch.h"
#include "Controller.h"

#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

Controller::Controller(float aMoveSpeed, float aRotSpeed)
{
	myMoveSpeed = aMoveSpeed;
	myRotSpeed = aRotSpeed;
}

void Controller::Start()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();
	inputHandler.RegisterAnalogAction("ObjectZMovement", Keys::S, Keys::W);
	inputHandler.RegisterAnalogAction("ObjectXMovement", Keys::A, Keys::D);
	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RMB", Keys::MOUSERBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("SPACE", Keys::SPACE, GenericInput::ActionType::Clicked);
	inputHandler.RegisterAnalog2DAction("MousePos", MouseMovement2D::MousePos);
	inputHandler.RegisterAnalog2DAction("MouseNDCPos", MouseMovement2D::MousePosNDC);
	inputHandler.RegisterAnalog2DAction("MouseDelta", MouseMovement2D::MousePosDelta);
}

void Controller::Update()
{
	float deltaTime = Engine::Get().GetTimer().GetDeltaTime();
	InputHandler& inputHandler = Engine::Get().GetInputHandler();

	Math::Vector3f inputDelta;

	inputDelta += gameObject->GetComponent<Transform>()->GetRightVector() * inputHandler.GetAnalogAction("ObjectXMovement");
	inputDelta += gameObject->GetComponent<Transform>()->GetForwardVector() * inputHandler.GetAnalogAction("ObjectZMovement");

	if (inputDelta.LengthSqr() > 1.0f)
	{
		inputDelta.Normalize();
	}


	//Math::Vector3<float> rotationDelta;
	//rotationDelta.y = inputHandler.GetAnalogAction("ObjectYRotation");

	//rotationDelta *= myRotSpeed * deltaTime;

	//gameObject->GetComponent<Transform>()->AddRotation(rotationDelta);
	gameObject->GetComponent<Transform>()->SetTranslation(gameObject->GetComponent<Transform>()->GetTranslation() + inputDelta * myMoveSpeed * deltaTime);
}
