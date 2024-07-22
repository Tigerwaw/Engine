#include "ComponentSystem/GameObject.h"
#include "Engine.h"
#include "GameEngine/Time/Timer.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
	myComponents.clear();
}

void GameObject::Update()
{
	if (!GetActive()) return;

	myTimeAlive += Engine::GetInstance().GetTimer().GetDeltaTime();

	for (auto& comp : myComponents)
	{
		if (comp->GetActive())
		{
			comp->Update();
		}
	}
}

void GameObject::SetActive(bool aActive)
{
	myIsActive = aActive;

	for (auto& comp : myComponents)
	{
		comp->SetActive(aActive);
	}
}

void GameObject::SetStatic(bool aStatic)
{
	myIsStatic = aStatic;
}

void GameObject::SendEvent(const GameObjectEvent aEvent)
{
	for (auto& comp : myComponents)
	{
		comp->ReceiveEvent(aEvent);
	}
}

void GameObject::SendEvent(const GameObjectEventType aEvent, std::any aValue)
{
	SendEvent(GameObjectEvent(aEvent, aValue));
}
