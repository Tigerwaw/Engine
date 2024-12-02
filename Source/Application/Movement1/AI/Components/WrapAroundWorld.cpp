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
	float offset = 10.0f;

	if (pos.x < -viewport.x - offset)
	{
		pos.x = viewport.x + offset;
	}
	else if (pos.x > viewport.x + offset)
	{
		pos.x = -viewport.x - offset;
	}

	if (pos.z < -viewport.y - offset)
	{
		pos.z = viewport.y + offset;
	}
	else if (pos.z > viewport.y + offset)
	{
		pos.z = -viewport.y - offset;
	}

	transform->SetTranslation(pos);
}
