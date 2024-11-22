#pragma once
#include "ControllerBase.h"

class Wander : public ControllerBase
{
public:
	void Start() override;
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	CU::Vector3f GetNewTargetRotation();
	float Random();
	float RandomBinomial();

	float myMaxRotation = 180.0f;
};