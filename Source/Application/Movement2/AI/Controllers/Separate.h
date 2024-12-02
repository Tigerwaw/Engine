#pragma once
#include "ControllerBase.h"

class Separate : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float myAvoidRadius = 100.0f;
	float myAvoidFactor = 10.0f;
};