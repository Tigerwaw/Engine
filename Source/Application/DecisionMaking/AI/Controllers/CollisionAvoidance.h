#pragma once
#include "ControllerBase.h"

class CollisionAvoidance : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;
private:
	float myAvoidRadius = 200.0f;
	float myAvoidFactor = 10.0f;
};