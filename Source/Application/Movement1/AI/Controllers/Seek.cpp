#include "Enginepch.h"
#include "Seek.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement1/AI/PollingStation.h"

ControllerBase::SteeringOutput Seek::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;
    output.velocity = PollingStation::Get().GetWandererPosition() - aSteeringInput.position;

    return output;
}