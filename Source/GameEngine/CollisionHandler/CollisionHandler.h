#pragma once

class Scene;

class CollisionHandler
{
public:
    CollisionHandler();
    ~CollisionHandler();

    void TestCollisions(Scene& aScene);
};

