#include "Enginepch.h"
#include "Seek.h"

ControllerBase::SteeringOutput Seek::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;
    output.velocity = aSteeringInput.target - aSteeringInput.position;

    return output;
}