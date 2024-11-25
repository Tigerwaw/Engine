#include "Enginepch.h"
#include "VelocityMatch.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement/AI/PollingStation.h"

ControllerBase::SteeringOutput VelocityMatch::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;
    output.velocity = (aSteeringInput.targetVelocity - aSteeringInput.velocity) / myTimeToTarget;
    output.rotation = 0;

    return output;
}