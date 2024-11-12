#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class ControllerBase
{
public:
	struct SteeringInput
	{
		CU::Vector3f position;
		CU::Vector3f orientation;
	};

	struct SteeringOutput
	{
		CU::Vector3f velocity;
		float rotation;
	};

	virtual ~ControllerBase();
	virtual void Start() = 0;
	virtual SteeringOutput GetSteering(const SteeringInput& aSteeringInput) = 0;
};