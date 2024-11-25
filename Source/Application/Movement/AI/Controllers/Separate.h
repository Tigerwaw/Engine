#pragma once
#include "ControllerBase.h"

class Separate : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float myNeighbourhoodRadius = 400.0f;
	float myAvoidanceThreshold = 50.0f;
};