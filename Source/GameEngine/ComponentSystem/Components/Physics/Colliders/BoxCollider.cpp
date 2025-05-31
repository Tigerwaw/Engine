#include "Enginepch.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Math/Intersection3D.hpp"
#include "Math/Vector.hpp"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "CommonUtilities/SerializationUtils.hpp"

BoxCollider::BoxCollider(Math::Vector3f aExtents, Math::Vector3f aCenterOffset)
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
    Math::AABB3D<float> otherAABBinMySpace = aCollider->GetAABB().GetAABBinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrix().GetFastInverse());

    return Math::IntersectionBetweenAABBS(GetAABB(), otherAABBinMySpace);
}

bool BoxCollider::TestCollision(const SphereCollider* aCollider) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    std::shared_ptr<Transform> collTransform = aCollider->gameObject->GetComponent<Transform>();
    Math::Sphere<float> sphereInMySpace = aCollider->GetSphere().GetSphereinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrix().GetFastInverse());

    Math::Vector3f intersectionPoint;
    return Math::IntersectionSphereAABB(sphereInMySpace, GetAABB(), intersectionPoint);
}

bool BoxCollider::TestCollision(const Math::Ray<float> aRay, Math::Vector3f& outHitPoint) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    Math::Ray<float> rayInMySpace = aRay.GetRayinNewSpace(transform->GetWorldMatrix().GetFastInverse());

    outHitPoint;
    return Math::IntersectionAABBRay(myAABB, rayInMySpace);
}

const Math::AABB3D<float>& BoxCollider::GetAABB() const
{
    return myAABB;
}

bool BoxCollider::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool BoxCollider::Deserialize(nl::json& aJsonObject)
{
    Math::Vector3f centerOffset;
    Math::Vector3f extents;

    if (aJsonObject.contains("CenterOffset"))
    {
        centerOffset = Utilities::DeserializeVector3<float>(aJsonObject["CenterOffset"]);
    }

    if (aJsonObject.contains("Extents"))
    {
        if (aJsonObject["Extents"].contains("Width"))
        {
            extents.x = aJsonObject["Extents"]["Width"].get<float>();
        }

        if (aJsonObject["Extents"].contains("Height"))
        {
            extents.y = aJsonObject["Extents"]["Height"].get<float>();
        }

        if (aJsonObject["Extents"].contains("Depth"))
        {
            extents.z = aJsonObject["Extents"]["Depth"].get<float>();
        }
    }

    myAABB.InitWithCenterAndExtents(centerOffset, extents);
    return true;
}
