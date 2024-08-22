#include "Enginepch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameEngine/Intersections/Intersection3D.hpp"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

SphereCollider::SphereCollider(float aRadius, CU::Vector3f aCenterOffset)
{
    mySphere.InitWithCenterAndRadius(aCenterOffset, aRadius);
}

bool SphereCollider::TestCollision(const Collider* aCollider) const
{
    return aCollider->TestCollision(this);
}

bool SphereCollider::TestCollision(const BoxCollider* aCollider) const
{
    return aCollider->TestCollision(this);
}

bool SphereCollider::TestCollision(const SphereCollider* aCollider) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    std::shared_ptr<Transform> collTransform = aCollider->gameObject->GetComponent<Transform>();
    CU::Sphere<float> sphereInWS = aCollider->GetSphere().GetSphereinNewSpace(collTransform->GetWorldMatrix());
    CU::Sphere<float> mySphereInWS = GetSphere().GetSphereinNewSpace(transform->GetWorldMatrix());

    return CU::IntersectionBetweenSpheres(sphereInWS, mySphereInWS);
}

const CU::Sphere<float>& SphereCollider::GetSphere() const
{
    return mySphere;
}