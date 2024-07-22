#include "Rotator.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

Rotator::Rotator(CU::Vector3<float> aRotationVector)
{
    SetRotationPerSecond(aRotationVector);
}

void Rotator::Start()
{
	myCurrentRot = CU::Quatf(gameObject->Transform.GetRotation() * 3.14f / 180.0f);
	myGoalRot = myCurrentRot;
}

void Rotator::Update()
{
	float deltaTime = Engine::GetInstance().GetTimer().GetDeltaTime();
    myCurrentRotationTime += deltaTime;

	if (myCurrentRotationTime >= myMaxRotationTime)
	{
		StartNewRotation();
	}

	float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
	CU::Quatf rot = CU::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
	gameObject->Transform.SetRotation(rot.GetEulerAnglesDegrees());
}

void Rotator::SetRotationPerSecond(CU::Vector3<float> aRotationVector)
{
	myRotationPerSecond = CU::Quatf(aRotationVector * 3.14f / 180.0f);
	StartNewRotation();
}

void Rotator::StartNewRotation()
{
	myCurrentRotationTime = 0;
	myCurrentRot = myGoalRot;
	myGoalRot = myCurrentRot * myRotationPerSecond;
}
