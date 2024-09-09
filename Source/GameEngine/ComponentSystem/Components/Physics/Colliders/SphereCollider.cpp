#include "Enginepch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameEngine/Intersections/Intersection3D.hpp"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

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
    CU::Sphere<float> otherSphereInMySpace = aCollider->GetSphere().GetSphereinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrix().GetFastInverse());

    return CU::IntersectionBetweenSpheres(GetSphere(), otherSphereInMySpace);
}

bool SphereCollider::TestCollision(const CU::Ray<float> aRay, CU::Vector3f& outHitPoint) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    CU::Ray<float> rayInMySpace = aRay.GetRayinNewSpace(transform->GetWorldMatrix().GetFastInverse());
    
    outHitPoint;
    return CU::IntersectionSphereRay(mySphere, rayInMySpace);
}

const CU::Sphere<float>& SphereCollider::GetSphere() const
{
    return mySphere;
}

bool SphereCollider::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool SphereCollider::Deserialize(nl::json& aJsonObject)
{
    CU::Vector3f centerOffset;
    float radius = 0;

    if (aJsonObject.contains("CenterOffset"))
    {
        centerOffset = Utility::DeserializeVector3<float>(aJsonObject["CenterOffset"]);
    }

    if (aJsonObject.contains("Radius"))
    {
        radius = aJsonObject["Radius"].get<float>();
    }

    mySphere.InitWithCenterAndRadius(centerOffset, radius);
    return true;
}
