#include "Enginepch.h"
#include "WalkToPoint.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Time/Timer.h"

WalkToPoint::WalkToPoint(float aMoveSpeed)
{
	SetMoveSpeed(aMoveSpeed);
}

void WalkToPoint::SetMoveSpeed(float aMoveSpeed)
{
	myMoveSpeed = aMoveSpeed;
}

void WalkToPoint::SetTarget(const CU::Vector3f& aPosition)
{
	myTargetPosition = aPosition;
}

void WalkToPoint::Start()
{
}

void WalkToPoint::Update()
{
	if (auto transform = gameObject->GetComponent<Transform>())
	{
		CU::Vector3f direction = myTargetPosition - transform->GetTranslation();
		if (direction.LengthSqr() < 1.0f) return;

		transform->SetTranslation(transform->GetTranslation() + direction.GetNormalized() * myMoveSpeed * Engine::Get().GetTimer().GetDeltaTime());
	}
}
