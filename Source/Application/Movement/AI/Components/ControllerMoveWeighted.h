#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector.hpp"
namespace CU = CommonUtilities;

class ControllerBase;

class ControllerMoveWeighted : public Component
{
public:
	enum class ControllerType
	{
		Separate,
		Cohesion,
		VelocityMatch,
		Wander
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

	const CU::Vector3f& GetVelocity() const { return myVelocity; }
	void SetTarget(std::shared_ptr<GameObject> aTarget) { myTarget = aTarget; }

protected:
	CU::Vector3f myVelocity;
	std::shared_ptr<GameObject> myTarget;

private:
	std::unordered_map<ControllerType, ControllerBase*> mySteeringBehaviours;
	std::unordered_map<ControllerType, float> mySteeringWeights;
	float myMaxAcceleration = 1.0f;
	float myDeceleration = 0.01f;
	float myMaxMoveSpeed = 10.0f;
};