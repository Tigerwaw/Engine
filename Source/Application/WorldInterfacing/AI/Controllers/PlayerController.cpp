#include "Enginepch.h"
#include "PlayerController.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Input/InputHandler.h"

#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/Math/Matrix.hpp"

void PlayerController::Start()
{
	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();

	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Clicked);
	inputHandler.RegisterAnalog2DAction("MousePosNDC", MouseMovement2D::MousePosNDC);
}

CU::Vector3f PlayerController::GetDirection(CU::Vector3f aCurrentPosition)
{
	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();

	if (inputHandler.GetBinaryAction("LMB"))
	{
		CU::Vector2f mousePosNDC = inputHandler.GetAnalogAction2D("MousePosNDC");

		auto cam = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Camera>();
		CU::Matrix4x4f invProj = cam->GetProjectionMatrix().GetInverse();
		CU::Vector4f eyeCoords = CU::Vector4f{ mousePosNDC.x, mousePosNDC.y, 0.0f, 1.0f } * invProj;

		eyeCoords /= eyeCoords.w;

		auto camTransform = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Transform>();
		CU::Matrix4x4f invView = camTransform->GetWorldMatrix();
		CU::Vector4f rayWorld = eyeCoords * invView;

		CU::Vector3f mousePosWorld = { rayWorld.x, rayWorld.y, rayWorld.z };
		myTargetPosition = mousePosWorld;
		myTargetPosition.y = 0;
	}

    return myTargetPosition - aCurrentPosition;
}
