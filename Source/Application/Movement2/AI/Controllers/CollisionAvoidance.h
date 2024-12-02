#pragma once
#include "ControllerBase.h"

class CollisionAvoidance : public ControllerBase
{
public:
	SteeringOutput GetSteering(const SteeringInput& aSteeringInput) override;

private:
	float myTurnFactor = 300.0f;
	float myEdgeMargin = 200.0f;
};