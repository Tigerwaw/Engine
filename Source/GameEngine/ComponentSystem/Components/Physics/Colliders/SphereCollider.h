#pragma once
#include "Collider.h"
#include "Math/Sphere.hpp"

class BoxCollider;

class SphereCollider : public Collider
{
public:
    SphereCollider(float aRadius = 100.0f, Math::Vector3f aCenterOffset = Math::Vector3f());

    bool CheckOverlap(const Collider* aCollider) const override;
    bool CheckOverlap(const BoxCollider* aCollider) const override;
    bool CheckOverlap(const SphereCollider* aCollider) const override;
    bool CheckOverlap(const Math::Ray<float> aRay, Math::Vector3f& outHitPoint) const override;

    const Math::Sphere<float>& GetSphere() const;

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
protected:
    Math::Sphere<float> mySphere;
};