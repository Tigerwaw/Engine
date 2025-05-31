#pragma once

class Scene;

class CollisionHandler
{
public:
    CollisionHandler();
    ~CollisionHandler();

    void TestCollisions(Scene& aScene);
    bool Raycast(Scene& aScene, Math::Vector3f aOrigin, Math::Vector3f aDirection, Math::Vector3f& aHitPoint);
};

