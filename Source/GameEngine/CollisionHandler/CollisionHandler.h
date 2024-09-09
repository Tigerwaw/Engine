#pragma once

class Scene;

class CollisionHandler
{
public:
    CollisionHandler();
    ~CollisionHandler();

    void TestCollisions(Scene& aScene);
    bool Raycast(Scene& aScene, CU::Vector3f aOrigin, CU::Vector3f aDirection, CU::Vector3f& aHitPoint);
};

