#include "Enginepch.h"
#include "MoveBetweenPoints.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

MoveBetweenPoints::MoveBetweenPoints(std::vector<CU::Vector3f> aPointList, float aMoveSpeed) : myPointList(aPointList), myMoveSpeed(aMoveSpeed)
{
}

void MoveBetweenPoints::SetPointList(std::vector<CU::Vector3f> aPointList)
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

	CU::Vector3f newPosition = CU::Vector3f::Lerp(myPointList[myLastGoal], myPointList[myCurrentGoal], t);
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
			myPointList.emplace_back(Utility::DeserializeVector3<float>(point));
		}
	}
	
	return true;
}
