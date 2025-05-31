#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"

class ControllerBase;

class ControllerMove : public Component
{
public:
	enum class ControllerType
	{
		Wander,
		Seek,
		Separate
	};

	ControllerMove() = default;
	~ControllerMove();
	ControllerMove(float aMaxMoveSpeed, float aMaxAcceleration, ControllerType aControllerType);

	void SetMaxMoveSpeed(float aMaxMoveSpeed);
	void SetMaxAcceleration(float aMaxAcceleration);
	void SetControllerType(ControllerType aControllerType);

	void Start() override;
	void Update() override;

protected:
	Math::Vector3f myVelocity;

private:
	ControllerBase* myController;
	float myMaxAcceleration = 1.0f;
	float myDeceleration = 0.01f;
	float myMaxMoveSpeed = 10.0f;

	bool myOrientTowardsVelocity = true;
	float myCurrentRotationTime = 0;
	float myMaxRotationTime = 0.25f;
	Math::Quatf myCurrentRot;
	Math::Quatf myGoalRot;
};