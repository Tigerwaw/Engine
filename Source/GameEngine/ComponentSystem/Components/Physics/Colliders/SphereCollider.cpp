#include "Enginepch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Math/Intersection3D.hpp"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "CommonUtilities/SerializationUtils.hpp"

SphereCollider::SphereCollider(float aRadius, Math::Vector3f aCenterOffset)
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
    Math::Sphere<float> otherSphereInMySpace = aCollider->GetSphere().GetSphereinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrix().GetFastInverse());

    return Math::IntersectionBetweenSpheres(GetSphere(), otherSphereInMySpace);
}

bool SphereCollider::TestCollision(const Math::Ray<float> aRay, Math::Vector3f& outHitPoint) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    Math::Ray<float> rayInMySpace = aRay.GetRayinNewSpace(transform->GetWorldMatrix().GetFastInverse());
    
    outHitPoint;
    return Math::IntersectionSphereRay(mySphere, rayInMySpace);
}

const Math::Sphere<float>& SphereCollider::GetSphere() const
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
    Math::Vector3f centerOffset;
    float radius = 0;

    if (aJsonObject.contains("CenterOffset"))
    {
        centerOffset = Utilities::DeserializeVector3<float>(aJsonObject["CenterOffset"]);
    }

    if (aJsonObject.contains("Radius"))
    {
        radius = aJsonObject["Radius"].get<float>();
    }

    mySphere.InitWithCenterAndRadius(centerOffset, radius);
    return true;
}
