#include "Enginepch.h"
#include "BounceAgainstWorldEdges.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "RandomDirectionMovement.h"

void BounceAgainstWorldEdges::Update()
{
	auto transform = gameObject->GetComponent<Transform>();
	auto pos = transform->GetTranslation(true);
	auto randomDir = gameObject->GetComponent<RandomDirectionMovement>();
	Math::Vector3f dir = randomDir->GetDirection();

	if (pos.x < -myExtents.x || pos.x > myExtents.x)
	{
		dir.x = -dir.x;
	}

	if (pos.z < -myExtents.z || pos.z > myExtents.z)
	{
		dir.z = -dir.z;
	}

	randomDir->SetDirection(dir);
}
