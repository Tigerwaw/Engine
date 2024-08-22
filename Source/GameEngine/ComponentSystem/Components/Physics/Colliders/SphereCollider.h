#pragma once
#include "Collider.h"
#include "GameEngine/Intersections/Sphere.hpp"
namespace CU = CommonUtilities;

class BoxCollider;

class SphereCollider : public Collider
{
public:
    SphereCollider(float aRadius = 100.0f, CU::Vector3f aCenterOffset = CU::Vector3f());

    bool TestCollision(const Collider* aCollider) const override;
    bool TestCollision(const BoxCollider* aCollider) const override;
    bool TestCollision(const SphereCollider* aCollider) const override;

    const CU::Sphere<float>& GetSphere() const;
protected:
    CU::Sphere<float> mySphere;
};