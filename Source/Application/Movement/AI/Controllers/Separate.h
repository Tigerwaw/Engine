#pragma once
#include "ControllerBase.h"

class Separate : public ControllerBase
{
public:
	void Start() override;
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float myThreshold = 300.0f;
};