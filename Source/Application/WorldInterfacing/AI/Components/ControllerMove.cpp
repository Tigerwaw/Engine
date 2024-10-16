#include "Enginepch.h"
#include "ControllerMove.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

#include "../Controllers/ControllerBase.h"
#include "../Controllers/PlayerController.h"
#include "../Controllers/AIEventsController.h"
#include "../Controllers/AIPollingController.h"

ControllerMove::~ControllerMove()
{
	delete myController;
}

ControllerMove::ControllerMove(float aMoveSpeed, ControllerType aControllerType)
{
	SetMoveSpeed(aMoveSpeed);
	SetControllerType(aControllerType);
}

void ControllerMove::SetMoveSpeed(float aMoveSpeed)
{
	myMoveSpeed = aMoveSpeed;
}

void ControllerMove::SetControllerType(ControllerType aControllerType)
{
	switch (aControllerType)
	{
	case ControllerMove::ControllerType::Player:
		myController = new PlayerController();
		break;
	case ControllerMove::ControllerType::AIEvents:
		myController = new AIEventsController();
		break;
	case ControllerMove::ControllerType::AIPolling:
		myController = new AIPollingController();
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
	auto& transform = gameObject->GetComponent<Transform>();
	CU::Vector3f currentPosition = transform->GetTranslation();
	CU::Vector3f direction = myController->GetDirection(currentPosition);
	if (direction.LengthSqr() > 1.0f)
	{
		CU::Vector3f newPosition = currentPosition + direction.GetNormalized() * myMoveSpeed * Engine::GetInstance().GetTimer().GetDeltaTime();
		transform->SetTranslation(newPosition);
	}
}
