#pragma once
#include "ControllerBase.h"

class Seek : public ControllerBase
{
public:
	void Start() override;
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;
};