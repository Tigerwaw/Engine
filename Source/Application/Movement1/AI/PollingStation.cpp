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

void PollingStation::AddWatchedActor(std::shared_ptr<GameObject> aGameObject)
{
	if (std::find(myWatchedActors.begin(), myWatchedActors.end(), aGameObject) != myWatchedActors.end()) return;

	myWatchedActors.emplace_back(aGameObject);
}

void PollingStation::SetWanderer(std::shared_ptr<GameObject> aGameObject)
{
	myWanderer = aGameObject;
}

const std::vector<Math::Vector3f> PollingStation::GetOtherActorPositions() const
{
	std::vector<Math::Vector3f> actorPositions;

	for (auto& actor : myWatchedActors)
	{
		if (auto transform = actor->GetComponent<Transform>())
		{
			actorPositions.emplace_back(transform->GetTranslation());
		}
	}

	return actorPositions;
}

const Math::Vector3f PollingStation::GetWandererPosition() const
{
	if (auto transform = myWanderer->GetComponent<Transform>())
	{
		return transform->GetTranslation();
	}

	return Math::Vector3f();
}
