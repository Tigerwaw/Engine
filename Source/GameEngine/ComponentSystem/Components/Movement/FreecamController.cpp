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

	if (!inputHandler.GetKeyDown(static_cast<int>(Keys::MOUSERBUTTON)))
	{
		return;
	}

	if (inputHandler.GetKeyClicked(static_cast<int>(Keys::SHIFT)))
	{
		myMoveSpeedMultiplier = 2.0f;
	}
	else if (inputHandler.GetKeyReleased(static_cast<int>(Keys::SHIFT)))
	{
		myMoveSpeedMultiplier = 1.0f;
	}

	if (inputHandler.GetKeyClicked(static_cast<int>(Keys::R)))
	{
		myParent->Transform.SetRotation(0, 0, 0);
		myParent->Transform.SetTranslation(0, 0, 0);
	}

	CU::Vector3<float> inputDelta;

	if (inputHandler.GetKeyDown(static_cast<int>(Keys::D)) || inputHandler.GetKeyDown(static_cast<int>(Keys::RIGHT)))
	{
		inputDelta += myParent->Transform.GetRightVector();
	}
	else if (inputHandler.GetKeyDown(static_cast<int>(Keys::A)) || inputHandler.GetKeyDown(static_cast<int>(Keys::LEFT)))
	{
		inputDelta -= myParent->Transform.GetRightVector();
	}

	if (inputHandler.GetKeyDown(static_cast<int>(Keys::W)) || inputHandler.GetKeyDown(static_cast<int>(Keys::UP)))
	{
		inputDelta += myParent->Transform.GetForwardVector();
	}
	else if (inputHandler.GetKeyDown(static_cast<int>(Keys::S)) || inputHandler.GetKeyDown(static_cast<int>(Keys::DOWN)))
	{
		inputDelta -= myParent->Transform.GetForwardVector();
	}

	if (inputHandler.GetKeyDown(static_cast<int>(Keys::SPACE)))
	{
		inputDelta += myParent->Transform.GetUpVector();
	}
	else if (inputHandler.GetKeyDown(static_cast<int>(Keys::CONTROL)))
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