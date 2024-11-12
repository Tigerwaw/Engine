#include "Enginepch.h"
#include "Separate.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "WorldInterfacing/AI/PollingStation.h"

void Separate::Start()
{
}

ControllerBase::SteeringOutput Separate::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;

    auto otherActors = PollingStation::Get().GetOtherActorPositions();
    for (auto& actorPos : otherActors)
    {
        CU::Vector3f diff = aSteeringInput.position - actorPos;
        if (diff.LengthSqr() < myThreshold * myThreshold)
        {
            output.velocity += diff;
        }
    }

    output.rotation = std::atan2f(-output.velocity.x, output.velocity.z);

    return output;
}