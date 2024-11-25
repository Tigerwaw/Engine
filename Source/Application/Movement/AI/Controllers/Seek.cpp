#include "Enginepch.h"
#include "Seek.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement/AI/PollingStation.h"

void Seek::Start()
{
}

ControllerBase::SteeringOutput Seek::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;
    output.velocity = PollingStation::Get().GetWandererPosition() - aSteeringInput.position;
    output.rotation = std::atan2f(-output.velocity.x, output.velocity.z);

    return output;
}