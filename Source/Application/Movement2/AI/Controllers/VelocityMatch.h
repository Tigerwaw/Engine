#pragma once
#include "ControllerBase.h"

class VelocityMatch : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float myNeighbourhoodRadius = 250.0f;
};