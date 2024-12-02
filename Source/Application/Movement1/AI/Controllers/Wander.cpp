#include "Enginepch.h"
#include "Wander.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement1/AI/PollingStation.h"

#include <random>

ControllerBase::SteeringOutput Wander::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;
    output.velocity = aSteeringInput.orientation;
    output.rotation = myMaxRotation * RandomBinomial();

    return output;
}

float Wander::Random()
{
    return (std::rand() % 100) / 100.0f;
}

float Wander::RandomBinomial()
{
    return Random() - Random();
}
