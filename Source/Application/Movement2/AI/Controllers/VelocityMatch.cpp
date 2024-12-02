#include "Enginepch.h"
#include "VelocityMatch.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement2/AI/PollingStation.h"
#include "Movement2/AI/Components/ControllerMoveWeighted.h"

ControllerBase::SteeringOutput VelocityMatch::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;
    aSteeringInput;

    auto otherActors = PollingStation::Get().GetOtherActorPositions();
    CU::Vector3f averageVelocity;
    float nearbyCount = 0;
    for (auto& actor : PollingStation::Get().GetOtherActors())
    {
        CU::Vector3f diff = aSteeringInput.position - actor->GetComponent<Transform>()->GetTranslation();
        if (diff.LengthSqr() > myNeighbourhoodRadius * myNeighbourhoodRadius) continue;
        if (auto& cont = actor->GetComponent<ControllerMoveWeighted>())
        {
            averageVelocity += cont->GetVelocity();
            nearbyCount++;
        }
    }

    averageVelocity /= nearbyCount;

    output.velocity = averageVelocity;

    return output;
}