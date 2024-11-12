#include "Enginepch.h"
#include "WrapAroundWorld.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"

void WrapAroundWorld::Update()
{
	auto transform = gameObject->GetComponent<Transform>();
	auto pos = transform->GetTranslation(true);
	auto viewport = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Camera>()->GetViewportDimensions();

	if (pos.x < -viewport.x)
	{
		pos.x = viewport.x;
	}
	else if (pos.x > viewport.x)
	{
		pos.x = -viewport.x;
	}

	if (pos.z < -viewport.y)
	{
		pos.z = viewport.y;
	}
	else if (pos.z > viewport.y)
	{
		pos.z = -viewport.y;
	}

	transform->SetTranslation(pos);
}
