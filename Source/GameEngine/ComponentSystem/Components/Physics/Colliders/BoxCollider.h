#pragma once
#include "Collider.h"
#include "GameEngine/Intersections/AABB3D.hpp"
namespace CU = CommonUtilities;

class SphereCollider;

class BoxCollider : public Collider
{
public:
    BoxCollider(CU::Vector3f aExtents = CU::Vector3f(1.0f, 1.0f, 1.0f), CU::Vector3f aCenterOffset = CU::Vector3f());

    bool TestCollision(const Collider* aCollider) const override;
    bool TestCollision(const BoxCollider* aCollider) const override;
    bool TestCollision(const SphereCollider* aCollider) const override;
    bool TestCollision(const CU::Ray<float> aRay, CU::Vector3f& outHitPoint) const override;

    const CU::AABB3D<float>& GetAABB() const;

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
protected:
    CU::AABB3D<float> myAABB;
};

