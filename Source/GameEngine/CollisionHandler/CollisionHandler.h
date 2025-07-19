#pragma once
#include "ComponentSystem/Components/Physics/Colliders/Collider.h"

class Scene;

class CollisionHandler
{
public:
    struct CollisionInfo
    {
        bool isTrigger = false;
        std::shared_ptr<Collider> colliderOne;
        std::shared_ptr<Collider> colliderTwo;

        bool operator==(const CollisionInfo& aOtherInfo)
        {
            return (colliderOne == aOtherInfo.colliderOne && colliderTwo == aOtherInfo.colliderTwo)
                || (colliderOne == aOtherInfo.colliderTwo && colliderTwo == aOtherInfo.colliderOne);
        }
    };

    CollisionHandler();
    ~CollisionHandler();

    void UpdateCollisions(Scene& aScene);
    bool Raycast(Scene& aScene, Math::Vector3f aOrigin, Math::Vector3f aDirection, Math::Vector3f& aHitPoint);

private:
    void AddActiveCollisions(Scene& aScene);
    void CompareCollisions();

    std::vector<CollisionInfo> myActiveCollisionsThisFrame;
    std::vector<CollisionInfo> myActiveCollisionsLastFrame;
};

