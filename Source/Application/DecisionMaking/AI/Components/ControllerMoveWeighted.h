#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector.hpp"

class ControllerBase;

class ControllerMoveWeighted : public Component
{
public:
	enum class ControllerType
	{
		Separate,
		Cohesion,
		Seek,
		Wander,
		CollisionAvoidance
	};

	ControllerMoveWeighted() = default;
	~ControllerMoveWeighted();
	ControllerMoveWeighted(float aMaxMoveSpeed, float aMaxAcceleration);

	void SetMaxMoveSpeed(float aMaxMoveSpeed);
	void SetMaxAcceleration(float aMaxAcceleration);
	void AddControllerType(ControllerType aControllerType, float aBlendWeight);
	void RemoveControllerType(ControllerType aControllerType);

	void Start() override;
	void Update() override;

	const Math::Vector3f& GetVelocity() const { return myVelocity; }

protected:
	Math::Vector3f myVelocity;

private:
	std::unordered_map<ControllerType, ControllerBase*> mySteeringBehaviours;
	std::unordered_map<ControllerType, float> mySteeringWeights;
	float myMaxAcceleration = 1.0f;
	float myDeceleration = 0.01f;
	float myMaxMoveSpeed = 10.0f;
	float myMinMoveSpeed = 5.0f;

	bool myOrientTowardsVelocity = true;
	float myCurrentRotationTime = 0;
	float myMaxRotationTime = 0.25f;
	Math::Quatf myCurrentRot;
	Math::Quatf myGoalRot;
};