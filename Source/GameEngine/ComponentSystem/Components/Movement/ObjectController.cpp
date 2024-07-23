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
}

void ObjectController::Update()
{
	float deltaTime = Engine::GetInstance().GetTimer().GetDeltaTime();
	CU::InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();

	if (inputHandler.GetKeyClicked(Keys::SHIFT))
	{
		myMoveSpeedMultiplier = 2.0f;
	}
	else if (inputHandler.GetKeyReleased(Keys::SHIFT))
	{
		myMoveSpeedMultiplier = 1.0f;
	}

	if (inputHandler.GetKeyClicked(Keys::R))
	{
		gameObject->GetComponent<Transform>()->SetRotation(0, 0, 0);
		gameObject->GetComponent<Transform>()->SetTranslation(0, 0, 0);
	}

	CU::Vector3<float> inputDelta;

	if (inputHandler.GetKeyDown(Keys::D) || inputHandler.GetKeyDown(Keys::RIGHT))
	{
		inputDelta += gameObject->GetComponent<Transform>()->GetRightVector();
	}
	else if (inputHandler.GetKeyDown(Keys::A) || inputHandler.GetKeyDown(Keys::LEFT))
	{
		inputDelta -= gameObject->GetComponent<Transform>()->GetRightVector();
	}

	if (inputHandler.GetKeyDown(Keys::W) || inputHandler.GetKeyDown(Keys::UP))
	{
		inputDelta += gameObject->GetComponent<Transform>()->GetForwardVector();
	}
	else if (inputHandler.GetKeyDown(Keys::S) || inputHandler.GetKeyDown(Keys::DOWN))
	{
		inputDelta -= gameObject->GetComponent<Transform>()->GetForwardVector();
	}

	if (inputHandler.GetKeyDown(Keys::SPACE))
	{
		inputDelta += gameObject->GetComponent<Transform>()->GetUpVector();
	}
	else if (inputHandler.GetKeyDown(Keys::CONTROL))
	{
		inputDelta -= gameObject->GetComponent<Transform>()->GetUpVector();
	}

	if (inputDelta.LengthSqr() > 1.0f)
	{
		inputDelta.Normalize();
	}


	CU::Vector3<float> rotationDelta;

	if (inputHandler.GetKeyDown(Keys::Q))
	{
		rotationDelta.y -= 1.0f;
	}
	else if (inputHandler.GetKeyDown(Keys::E))
	{
		rotationDelta.y += 1.0f;
	}

	rotationDelta *= myRotSpeed * deltaTime;

	gameObject->GetComponent<Transform>()->AddRotation(rotationDelta);
	gameObject->GetComponent<Transform>()->SetTranslation(gameObject->GetComponent<Transform>()->GetTranslation() + inputDelta * myMoveSpeed * myMoveSpeedMultiplier * deltaTime);
}
