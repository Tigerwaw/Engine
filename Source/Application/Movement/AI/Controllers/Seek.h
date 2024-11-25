#pragma once
#include "ControllerBase.h"

class Seek : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;
};