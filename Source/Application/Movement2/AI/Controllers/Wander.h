#pragma once
#include "ControllerBase.h"

class Wander : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float Random();
	float RandomBinomial();

	float myMaxRotation = 360.0f;
};