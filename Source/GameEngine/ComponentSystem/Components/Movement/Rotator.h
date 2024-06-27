#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Quaternion.hpp"

namespace CU = CommonUtilities;

class Rotator : public Component
{
public:
	Rotator() = default;
    Rotator(CU::Vector3<float> aRotationVector);
	
	void Start() override;
	void Update() override;

	void SetRotationPerSecond(CU::Vector3<float> aRotationVector);

private:
	void StartNewRotation();
	float myCurrentRotationTime = 0;
	float myMaxRotationTime = 1.0;
	CU::Quatf myRotationPerSecond;
	CU::Quatf myCurrentRot;
	CU::Quatf myGoalRot;
};

