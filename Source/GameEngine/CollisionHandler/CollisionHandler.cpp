#include "Enginepch.h"
#include "CollisionHandler.h"
#include "ComponentSystem/Scene.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Component.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Physics/Colliders/Collider.h"
#include "Math/Intersection3D.hpp"

CollisionHandler::CollisionHandler()
{
}

CollisionHandler::~CollisionHandler()
{
}

void CollisionHandler::TestCollisions(Scene& aScene)
{
	PIXScopedEvent(PIX_COLOR_INDEX(9), "Test Collisions in Scene");

    for (auto& goA : aScene.myGameObjects)
    {
		int collisions = 0;
		if (!goA->GetActive()) continue;

		std::shared_ptr<Collider> colliderA = goA->GetComponent<Collider>();
		if (!colliderA || !colliderA->GetActive()) continue;

		for (auto& goB : aScene.myGameObjects)
		{
			if (goA == goB) break;
			if (!goB->GetActive()) continue;

			std::shared_ptr<Collider> colliderB = goB->GetComponent<Collider>();
			if (!colliderB || !colliderB->GetActive()) continue;

			if (colliderA->TestCollision(colliderB.get()))
			{
				colliderA->TriggerCollisionResponse();
				colliderB->TriggerCollisionResponse();
				colliderA->debugColliding = true;
				colliderB->debugColliding = true;
				collisions++;
			}
		}

		if (collisions == 0) colliderA->debugColliding = false;
    }
}

bool CollisionHandler::Raycast(Scene& aScene, Math::Vector3f aOrigin, Math::Vector3f aDirection, Math::Vector3f& aHitPoint)
{
	for (auto& goA : aScene.myGameObjects)
	{
		if (!goA->GetActive()) continue;

		std::shared_ptr<Collider> colliderA = goA->GetComponent<Collider>();
		if (!colliderA || !colliderA->GetActive()) continue;

		Math::Ray<float> aRay(aOrigin, aDirection);
		Math::Vector3f hitPoint;
		if (colliderA->TestCollision(aRay, hitPoint))
		{
			return true;
		}
	}

	aHitPoint = { 0, 0, 0 };
	return false;
}
