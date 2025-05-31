#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Quaternion.hpp"



class Rotator : public Component
{
public:
	Rotator() = default;
    Rotator(const Math::Vector3f& aRotationVector);
	
	void Start() override;
	void Update() override;

	void SetRotationPerSecond(const Math::Vector3f& aRotationVector);

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;

private:
	void StartNewRotation();
	float myCurrentRotationTime = 0;
	float myMaxRotationTime = 1.0;
	Math::Quatf myRotationPerSecond;
	Math::Quatf myCurrentRot;
	Math::Quatf myGoalRot;
};

