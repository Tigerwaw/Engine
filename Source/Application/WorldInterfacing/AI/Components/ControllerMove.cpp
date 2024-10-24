#include "Enginepch.h"
#include "ControllerMove.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

#include "WorldInterfacing/AI/Controllers/ControllerBase.h"
#include "WorldInterfacing/AI/Controllers/PlayerController.h"
#include "WorldInterfacing/AI/Controllers/AIEventsController.h"
#include "WorldInterfacing/AI/Controllers/AIPollingController.h"

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
