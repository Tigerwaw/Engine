#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Ray.hpp"
#include "Math/Intersection3D.hpp"

class BoxCollider;
class SphereCollider;

class Collider : public Component
{
    friend class CollisionHandler;
public:
    struct CollisionInfo
    {
        bool collided = false;
        Math::Vector3f hitPoint;

        operator bool() const
        {
            return collided;
        }

        CollisionInfo(const Math::IntersectionInfo& aInfo)
        {
            collided = aInfo.intersected;
            hitPoint = aInfo.intersectionPoint;
        }
    };

    void Start() override {}
    void Update() override {}
    void SetIsTrigger(bool aIsTrigger) { myIsTrigger = aIsTrigger; }
    bool IsTrigger() const { return myIsTrigger; }

    bool IsOverlapping() const { return myIsOverlappingDebug; }

    virtual CollisionInfo CheckOverlap(const Collider* aCollider) const = 0;
    virtual CollisionInfo CheckOverlap(const BoxCollider* aCollider) const = 0;
    virtual CollisionInfo CheckOverlap(const SphereCollider* aCollider) const = 0;
    virtual CollisionInfo CheckOverlap(const Math::Ray<float> aRay) const = 0;

    void SetOnCollisionEnterResponse(const std::function<void()>& aCallback);
    void SetOnCollisionStayResponse(const std::function<void()>& aCallback);
    void SetOnCollisionExitResponse(const std::function<void()>& aCallback);

    void SetOnTriggerEnterResponse(const std::function<void()>& aCallback);
    void SetOnTriggerStayResponse(const std::function<void()>& aCallback);
    void SetOnTriggerExitResponse(const std::function<void()>& aCallback);

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;

private:
    bool myIsTrigger = false;
    bool myIsOverlappingDebug = false;

    void OnCollisionEnter();
    void OnCollisionStay();
    void OnCollisionExit();

    void OnTriggerEnter();
    void OnTriggerStay();
    void OnTriggerExit();
    std::function<void()> myOnCollisionEnterResponse;
    std::function<void()> myOnCollisionStayResponse;
    std::function<void()> myOnCollisionExitResponse;
    std::function<void()> myOnTriggerEnterResponse;
    std::function<void()> myOnTriggerStayResponse;
    std::function<void()> myOnTriggerExitResponse;
};

