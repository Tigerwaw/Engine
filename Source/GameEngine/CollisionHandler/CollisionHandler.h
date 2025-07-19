#pragma once
#include "ComponentSystem/Components/Physics/Colliders/Collider.h"

class Scene;

class CollisionHandler
{
public:
    struct CollisionHandle
    {
        bool isTrigger = false;
        std::shared_ptr<Collider> colliderOne;
        std::shared_ptr<Collider> colliderTwo;

        bool operator==(const CollisionHandle& aOtherHandle)
        {
            return (colliderOne == aOtherHandle.colliderOne && colliderTwo == aOtherHandle.colliderTwo)
                || (colliderOne == aOtherHandle.colliderTwo && colliderTwo == aOtherHandle.colliderOne);
        }
    };

    CollisionHandler();
    ~CollisionHandler();

    void UpdateCollisions(Scene& aScene);
    bool Raycast(Scene& aScene, Math::Vector3f aOrigin, Math::Vector3f aDirection, Math::Vector3f& aHitPoint);

private:
    void AddActiveCollisions(Scene& aScene);
    void CompareCollisions();

    std::vector<CollisionHandle> myActiveCollisionsThisFrame;
    std::vector<CollisionHandle> myActiveCollisionsLastFrame;
};

