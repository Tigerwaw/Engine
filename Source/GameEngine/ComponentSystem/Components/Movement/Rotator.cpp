#include "Enginepch.h"

#include "Rotator.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "Engine.h"
#include "Time/Timer.h"
#include "CommonUtilities/SerializationUtils.hpp"

Rotator::Rotator(const Math::Vector3f& aRotationVector)
{
    SetRotationPerSecond(aRotationVector);
}

void Rotator::Start()
{
	myCurrentRot = Math::Quatf(gameObject->GetComponent<Transform>()->GetRotation() * 3.14f / 180.0f);
	myGoalRot = myCurrentRot;
}

void Rotator::Update()
{
	float deltaTime = Engine::Get().GetTimer().GetDeltaTime();
    myCurrentRotationTime += deltaTime;

	if (myCurrentRotationTime >= myMaxRotationTime)
	{
		StartNewRotation();
	}

	float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
	Math::Quatf rot = Math::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
	gameObject->GetComponent<Transform>()->SetRotation(rot.GetEulerAnglesDegrees());
}

void Rotator::SetRotationPerSecond(const Math::Vector3f& aRotationVector)
{
	myRotationPerSecond = Math::Quatf(aRotationVector * 3.14f / 180.0f);
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
		SetRotationPerSecond(Utilities::DeserializeVector3<float>(aJsonObject["RotationPerSecond"]));
	}

	return true;
}

void Rotator::StartNewRotation()
{
	myCurrentRotationTime = 0;
	myCurrentRot = myGoalRot;
	myGoalRot = myCurrentRot * myRotationPerSecond;
}
