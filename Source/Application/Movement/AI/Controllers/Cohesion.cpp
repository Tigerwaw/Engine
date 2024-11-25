#include "Enginepch.h"
#include "Cohesion.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement/AI/PollingStation.h"

ControllerBase::SteeringOutput Cohesion::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;

    auto otherActors = PollingStation::Get().GetOtherActorPositions();
    CU::Vector3f averagePos;
    float nearbyCount = 0;
    for (auto& actorPos : otherActors)
    {
        CU::Vector3f diff = aSteeringInput.position - actorPos;
        if (diff.LengthSqr() > myNeighbourhoodRadius * myNeighbourhoodRadius) continue;

        averagePos -= diff;
        nearbyCount++;
    }

    averagePos /= nearbyCount;

    output.rotation = 0;
    return output;
}