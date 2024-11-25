#pragma once
#include "ControllerBase.h"

class Wander : public ControllerBase
{
public:
	void Start() override;
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float Random();
	float RandomBinomial();

	float myMaxRotation = 180.0f;
};