#include "Enginepch.h"
#include "PollingStation.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

PollingStation::PollingStation()
{
}

PollingStation::~PollingStation()
{
}

void PollingStation::Update()
{
}

std::shared_ptr<GameObject> PollingStation::GetRandomAIActor(std::shared_ptr<GameObject> aSelf)
{
	int random = 0;
	do 
	{
		random = std::rand() % 3;
	} while (myAIActors[random] == aSelf);

	return myAIActors[random];
}

void PollingStation::AddAIActor(std::shared_ptr<GameObject> aGameObject)
{
	myAIActors.push_back(aGameObject);
}

const CU::Vector3f PollingStation::GetHealingWellPosition()
{
	return myHealingWell->GetComponent<Transform>()->GetTranslation(true);
}

const std::vector<CU::Vector3f> PollingStation::GetWallPositions() const
{
	std::vector<CU::Vector3f> wallPositions;

	for (auto& wall : myWalls)
	{
		if (auto transform = wall->GetComponent<Transform>())
		{
			wallPositions.emplace_back(transform->GetTranslation());
		}
	}

	return wallPositions;
}
