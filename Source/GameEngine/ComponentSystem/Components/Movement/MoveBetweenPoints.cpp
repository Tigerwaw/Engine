#include "Enginepch.h"
#include "MoveBetweenPoints.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "Engine.h"
#include "Time/Timer.h"
#include "CommonUtilities/SerializationUtils.hpp"

MoveBetweenPoints::MoveBetweenPoints(std::vector<Math::Vector3f> aPointList, float aMoveSpeed) : myPointList(aPointList), myMoveSpeed(aMoveSpeed)
{
}

void MoveBetweenPoints::SetPointList(std::vector<Math::Vector3f> aPointList)
{
	myPointList = aPointList;
}

void MoveBetweenPoints::SetMoveSpeed(float aMoveSpeed)
{
	myMoveSpeed = aMoveSpeed;
}

void MoveBetweenPoints::Start()
{
}

void MoveBetweenPoints::Update()
{
	float deltaTime = Engine::Get().GetTimer().GetDeltaTime();
	std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();

	myCurrentTimeMoved += deltaTime;
	float t = myCurrentTimeMoved / myTimeToNextGoal;

	Math::Vector3f newPosition = Math::Vector3f::Lerp(myPointList[myLastGoal], myPointList[myCurrentGoal], t);
	transform->SetTranslation(newPosition);
	
	if (t > 0.99f)
	{
		myLastGoal = myCurrentGoal;
		myCurrentGoal++;
		if (myCurrentGoal >= myPointList.size())
		{
			myCurrentGoal = 0;
		}

		myCurrentTimeMoved = 0;
		float distanceToNewGoal = (transform->GetTranslation(true) - myPointList[myCurrentGoal]).Length();
		myTimeToNextGoal = distanceToNewGoal / myMoveSpeed;
	}
}

bool MoveBetweenPoints::Serialize(nl::json&)
{
	return false;
}

bool MoveBetweenPoints::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Speed"))
	{
		myMoveSpeed = aJsonObject["Speed"].get<float>();
	}

	if (aJsonObject.contains("Points"))
	{
		for (auto& point : aJsonObject["Points"])
		{
			myPointList.emplace_back(Utilities::DeserializeVector3<float>(point));
		}
	}
	
	return true;
}
