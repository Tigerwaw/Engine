#pragma once
#include "ControllerBase.h"

class Separate : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float myThreshold = 200.0f;
};