#pragma once
#include "ControllerBase.h"

class Cohesion : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float myNeighbourhoodRadius = 800.0f;
};