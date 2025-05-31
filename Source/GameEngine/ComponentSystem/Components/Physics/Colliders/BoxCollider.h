#pragma once
#include "Collider.h"
#include "Math/AABB3D.hpp"


class SphereCollider;

class BoxCollider : public Collider
{
public:
    BoxCollider(Math::Vector3f aExtents = Math::Vector3f(1.0f, 1.0f, 1.0f), Math::Vector3f aCenterOffset = Math::Vector3f());

    bool TestCollision(const Collider* aCollider) const override;
    bool TestCollision(const BoxCollider* aCollider) const override;
    bool TestCollision(const SphereCollider* aCollider) const override;
    bool TestCollision(const Math::Ray<float> aRay, Math::Vector3f& outHitPoint) const override;

    const Math::AABB3D<float>& GetAABB() const;

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
protected:
    Math::AABB3D<float> myAABB;
};

