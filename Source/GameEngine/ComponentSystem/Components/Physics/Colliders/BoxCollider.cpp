#include "Enginepch.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "GameEngine/Intersections/Intersection3D.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

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
    CU::AABB3D<float> otherAABBinMySpace = aCollider->GetAABB().GetAABBinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrix().GetFastInverse());

    return CU::IntersectionBetweenAABBS(GetAABB(), otherAABBinMySpace);
}

bool BoxCollider::TestCollision(const SphereCollider* aCollider) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    std::shared_ptr<Transform> collTransform = aCollider->gameObject->GetComponent<Transform>();
    CU::Sphere<float> sphereInMySpace = aCollider->GetSphere().GetSphereinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrix().GetFastInverse());

    CU::Vector3f intersectionPoint;
    return CU::IntersectionSphereAABB(sphereInMySpace, GetAABB(), intersectionPoint);
}

const CU::AABB3D<float>& BoxCollider::GetAABB() const
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
    CU::Vector3f centerOffset;
    CU::Vector3f extents;

    if (aJsonObject.contains("CenterOffset"))
    {
        centerOffset = Utility::DeserializeVector3<float>(aJsonObject["CenterOffset"]);
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
