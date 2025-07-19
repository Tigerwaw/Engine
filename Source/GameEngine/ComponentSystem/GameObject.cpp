#include "Enginepch.h"

#include "ComponentSystem/GameObject.h"
#include "Engine.h"
#include "Time/Timer.h"

void GameObject::Update()
{
	if (!GetActive()) return;

	myTimeAlive += Engine::Get().GetTimer().GetDeltaTime();

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