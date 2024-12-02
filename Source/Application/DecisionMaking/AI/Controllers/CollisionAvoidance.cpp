#include "Enginepch.h"
#include "CollisionAvoidance.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "../PollingStation.h"

ControllerBase::SteeringOutput CollisionAvoidance::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;

    for (auto& wallPos : PollingStation::Get().GetWallPositions())
    {
        CU::Vector3f diff = aSteeringInput.position - wallPos;
        if (diff.LengthSqr() > myAvoidRadius * myAvoidRadius) continue;
        output.velocity += diff;
    }

    output.velocity *= myAvoidFactor;
    return output;
}