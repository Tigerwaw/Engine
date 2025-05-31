#pragma once
#include "Math/Vector.hpp"

class ControllerBase
{
public:
	struct SteeringInput
	{
		Math::Vector3f velocity;
		Math::Vector3f position;
		Math::Vector3f orientation;
		Math::Vector3f target;
	};

	struct SteeringOutput
	{
		Math::Vector3f velocity;
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