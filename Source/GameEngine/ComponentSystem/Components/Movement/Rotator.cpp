#include "Enginepch.h"

#include "Rotator.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

Rotator::Rotator(CU::Vector3<float> aRotationVector)
{
    SetRotationPerSecond(aRotationVector);
}

void Rotator::Start()
{
	myCurrentRot = CU::Quatf(gameObject->GetComponent<Transform>()->GetRotation() * 3.14f / 180.0f);
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
	gameObject->GetComponent<Transform>()->SetRotation(rot.GetEulerAnglesDegrees());
}

void Rotator::SetRotationPerSecond(CU::Vector3<float> aRotationVector)
{
	myRotationPerSecond = CU::Quatf(aRotationVector * 3.14f / 180.0f);
	StartNewRotation();
}

bool Rotator::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool Rotator::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("RotationPerSecond"))
	{
		SetRotationPerSecond(Utility::DeserializeVector3<float>(aJsonObject["RotationPerSecond"]));
	}

	return true;
}

void Rotator::StartNewRotation()
{
	myCurrentRotationTime = 0;
	myCurrentRot = myGoalRot;
	myGoalRot = myCurrentRot * myRotationPerSecond;
}
