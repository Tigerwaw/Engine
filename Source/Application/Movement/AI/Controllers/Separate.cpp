#include "Enginepch.h"
#include "Separate.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Movement/AI/PollingStation.h"

#include <random>

ControllerBase::SteeringOutput Separate::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;

    auto otherActors = PollingStation::Get().GetOtherActorPositions();
    for (auto& actorPos : otherActors)
    {
        CU::Vector3f diff = aSteeringInput.position - actorPos;
        float diffLength = diff.LengthSqr();
        if (diffLength > myNeighbourhoodRadius * myNeighbourhoodRadius) continue;
        if (diffLength > myAvoidanceThreshold * myAvoidanceThreshold) continue;
        if (diffLength < 1.0f)
        {
            diff = CU::Vector3f(static_cast<float>(std::rand() % 100), 0, static_cast<float>(std::rand() % 100));
        }

        output.velocity += diff;
    }

    output.rotation = 0;
    return output;
}