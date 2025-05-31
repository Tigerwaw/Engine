#include "Enginepch.h"
#include "Separate.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement2/AI/PollingStation.h"

#include <random>

ControllerBase::SteeringOutput Separate::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;

    auto otherActors = PollingStation::Get().GetOtherActorPositions();
    for (auto& actorPos : otherActors)
    {
        Math::Vector3f diff = aSteeringInput.position - actorPos;
        if (diff.LengthSqr() > myAvoidRadius * myAvoidRadius) continue;
        output.velocity += diff;
    }

    output.velocity *= myAvoidFactor;
    return output;
}