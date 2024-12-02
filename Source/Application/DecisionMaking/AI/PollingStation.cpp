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

const CU::Vector3f PollingStation::GetAIOnePosition()
{
	return myAIOne->GetComponent<Transform>()->GetTranslation(true);
}

const CU::Vector3f PollingStation::GetAITwoPosition()
{
	return myAITwo->GetComponent<Transform>()->GetTranslation(true);
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
