#include "Enginepch.h"
#include "CollisionHandler.h"
#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/ComponentSystem/Components/Physics/Colliders/Collider.h"

CollisionHandler::CollisionHandler()
{
}

CollisionHandler::~CollisionHandler()
{
}

void CollisionHandler::TestCollisions(Scene& aScene)
{
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
				// Send collision response
				colliderA->debugColliding = true;
				colliderB->debugColliding = true;
				collisions++;
			}
		}

		if (collisions == 0) colliderA->debugColliding = false;
    }
}