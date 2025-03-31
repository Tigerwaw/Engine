#include "Enginepch.h"
#include "CollisionHandler.h"
#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Physics/Colliders/Collider.h"
#include "Intersections\Intersection3D.hpp"

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

bool CollisionHandler::Raycast(Scene& aScene, CU::Vector3f aOrigin, CU::Vector3f aDirection, CU::Vector3f& aHitPoint)
{
	for (auto& goA : aScene.myGameObjects)
	{
		if (!goA->GetActive()) continue;

		std::shared_ptr<Collider> colliderA = goA->GetComponent<Collider>();
		if (!colliderA || !colliderA->GetActive()) continue;

		CU::Ray<float> aRay(aOrigin, aDirection);
		CU::Vector3f hitPoint;
		if (colliderA->TestCollision(aRay, hitPoint))
		{
			return true;
		}
	}

	aHitPoint = { 0, 0, 0 };
	return false;
}
