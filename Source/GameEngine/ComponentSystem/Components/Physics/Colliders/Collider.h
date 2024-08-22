#pragma once
#include "GameEngine/ComponentSystem/Component.h"

class BoxCollider;
class SphereCollider;

class Collider : public Component
{
public:
    void Start() override {}
    void Update() override {}

    virtual bool TestCollision(const Collider* aCollider) const = 0;
    virtual bool TestCollision(const BoxCollider* aCollider) const = 0;
    virtual bool TestCollision(const SphereCollider* aCollider) const = 0;

    bool debugColliding = false;
};

