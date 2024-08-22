#include "Enginepch.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "GameEngine/Intersections/Intersection3D.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

BoxCollider::BoxCollider(CU::Vector3f aExtents, CU::Vector3f aCenterOffset)
{
    myAABB.InitWithCenterAndExtents(aCenterOffset, aExtents);
}

bool BoxCollider::TestCollision(const Collider* aCollider) const
{
    return aCollider->TestCollision(this);
}

bool BoxCollider::TestCollision(const BoxCollider* aCollider) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    std::shared_ptr<Transform> collTransform = aCollider->gameObject->GetComponent<Transform>();
    CU::AABB3D<float> otherAABBinWS = aCollider->GetAABB().GetAABBinNewSpace(collTransform->GetWorldMatrix());
    CU::AABB3D<float> myAABBinWS = GetAABB().GetAABBinNewSpace(transform->GetWorldMatrix());

    return CU::IntersectionBetweenAABBS(myAABBinWS, otherAABBinWS);
}

bool BoxCollider::TestCollision(const SphereCollider* aCollider) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    std::shared_ptr<Transform> collTransform = aCollider->gameObject->GetComponent<Transform>();
    CU::Sphere<float> sphereInWS = aCollider->GetSphere().GetSphereinNewSpace(collTransform->GetWorldMatrix());
    CU::AABB3D<float> myAABBinWS = GetAABB().GetAABBinNewSpace(transform->GetWorldMatrix());

    CU::Vector3f intersectionPoint;
    return CU::IntersectionSphereAABB(sphereInWS, myAABBinWS, intersectionPoint);
}

const CU::AABB3D<float>& BoxCollider::GetAABB() const
{
    return myAABB;
}