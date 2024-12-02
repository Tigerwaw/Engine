#include "Enginepch.h"
#include "Separate.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement1/AI/PollingStation.h"

#include <random>

ControllerBase::SteeringOutput Separate::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;

    auto otherActors = PollingStation::Get().GetOtherActorPositions();
    for (auto& actorPos : otherActors)
    {
        if (CU::Vector3f::Equal(aSteeringInput.position, actorPos, 1.0f)) continue;
        CU::Vector3f diff = aSteeringInput.position - actorPos;
        if (diff.LengthSqr() < myThreshold * myThreshold)
        {
            output.velocity += diff;
        }
    }

    return output;
}