#include "FreecamController.h"
#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"

FreecamController::FreecamController(float aMoveSpeed, float aRotSpeed)
{
	myMoveSpeed = aMoveSpeed;
	myMoveSpeedMultiplier = 1.0f;
	myRotSpeed = { aRotSpeed, aRotSpeed * 0.5f };
}

void FreecamController::Start()
{
}

void FreecamController::Update()
{
	float deltaTime = Engine::GetInstance().GetTimer().GetDeltaTime();
	CU::InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();

	if (!inputHandler.GetKeyDown(Keys::MOUSERBUTTON))
	{
		return;
	}

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
		myParent->Transform.SetRotation(0, 0, 0);
		myParent->Transform.SetTranslation(0, 0, 0);
	}

	CU::Vector3<float> inputDelta;

	if (inputHandler.GetKeyDown(Keys::D) || inputHandler.GetKeyDown(Keys::RIGHT))
	{
		inputDelta += myParent->Transform.GetRightVector();
	}
	else if (inputHandler.GetKeyDown(Keys::A) || inputHandler.GetKeyDown(Keys::LEFT))
	{
		inputDelta -= myParent->Transform.GetRightVector();
	}

	if (inputHandler.GetKeyDown(Keys::W) || inputHandler.GetKeyDown(Keys::UP))
	{
		inputDelta += myParent->Transform.GetForwardVector();
	}
	else if (inputHandler.GetKeyDown(Keys::S) || inputHandler.GetKeyDown(Keys::DOWN))
	{
		inputDelta -= myParent->Transform.GetForwardVector();
	}

	if (inputHandler.GetKeyDown(Keys::SPACE))
	{
		inputDelta += myParent->Transform.GetUpVector();
	}
	else if (inputHandler.GetKeyDown(Keys::CONTROL))
	{
		inputDelta -= myParent->Transform.GetUpVector();
	}

	if (inputDelta.LengthSqr() > 1.0f)
	{
		inputDelta.Normalize();
	}


	CU::Vector3<float> rotationDelta;
	rotationDelta.x = static_cast<float>(inputHandler.GetMouseDelta().y);
	rotationDelta.y = static_cast<float>(inputHandler.GetMouseDelta().x);
	rotationDelta.z = 0;

	rotationDelta.x = abs(rotationDelta.x) < 1 ? 0 : rotationDelta.x;
	rotationDelta.y = abs(rotationDelta.y) < 1 ? 0 : rotationDelta.y;

	rotationDelta.x *= myRotSpeed.y * deltaTime;
	rotationDelta.y *= myRotSpeed.x * deltaTime;

	myParent->Transform.AddRotation(rotationDelta);
	myParent->Transform.SetTranslation(myParent->Transform.GetTranslation() + inputDelta * myMoveSpeed * myMoveSpeedMultiplier * deltaTime);
}