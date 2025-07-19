#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Ray.hpp"

class BoxCollider;
class SphereCollider;

class Collider : public Component
{
    friend class CollisionHandler;
public:
    void Start() override {}
    void Update() override {}
    void SetIsTrigger(bool aIsTrigger) { myIsTrigger = aIsTrigger; }
    bool IsTrigger() const { return myIsTrigger; }

    bool IsOverlapping() const { return myIsOverlappingDebug; }

    virtual bool CheckOverlap(const Collider* aCollider) const = 0;
    virtual bool CheckOverlap(const BoxCollider* aCollider) const = 0;
    virtual bool CheckOverlap(const SphereCollider* aCollider) const = 0;
    virtual bool CheckOverlap(const Math::Ray<float> aRay, Math::Vector3f& outHitPoint) const = 0;

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

