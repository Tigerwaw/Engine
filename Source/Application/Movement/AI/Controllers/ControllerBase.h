#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class ControllerBase
{
public:
	struct SteeringInput
	{
		CU::Vector3f velocity;
		CU::Vector3f position;
		CU::Vector3f orientation;
		CU::Vector3f targetVelocity;
		CU::Vector3f targetPosition;
	};

	struct SteeringOutput
	{
		CU::Vector3f velocity;
		float rotation = 0;

		SteeringOutput operator+=(SteeringOutput output)
		{
			velocity += output.velocity;
			rotation += output.rotation;

			return *this;
		}

		SteeringOutput operator*(float weight)
		{
			SteeringOutput steering;
			steering.velocity = velocity;
			steering.velocity *= weight;
			steering.rotation = rotation;
			steering.rotation *= weight;

			return steering;
		}
	};

	virtual ~ControllerBase();
	virtual SteeringOutput GetSteering(const SteeringInput& aSteeringInput) = 0;
};