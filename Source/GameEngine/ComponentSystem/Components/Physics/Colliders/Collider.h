#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Intersections/Ray.hpp"

class BoxCollider;
class SphereCollider;

class Collider : public Component
{
    friend class CollisionHandler;
public:
    void Start() override {}
    void Update() override {}

    virtual bool TestCollision(const Collider* aCollider) const = 0;
    virtual bool TestCollision(const BoxCollider* aCollider) const = 0;
    virtual bool TestCollision(const SphereCollider* aCollider) const = 0;
    virtual bool TestCollision(const CU::Ray<float> aRay, CU::Vector3f& outHitPoint) const = 0;

    void SetCollisionResponse(const std::function<void()>& aCallback);

    bool debugColliding = false;
private:
    void TriggerCollisionResponse() const;
    std::function<void()> myCollisionResponse;
};

