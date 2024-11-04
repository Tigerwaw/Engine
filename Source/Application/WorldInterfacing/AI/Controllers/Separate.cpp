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

CU::Vector3f Separate::GetDirection(CU::Vector3f aCurrentPosition)
{
    PollingStation& ps = PollingStation::Get();
    for (auto& otherActorPos : ps.GetOtherActorPositions())
    {
        if (otherActorPos == aCurrentPosition) continue;
        CU::Vector3f diff = otherActorPos - aCurrentPosition;
        float toleranceSqr = 200.0f;
        if (diff.LengthSqr() < toleranceSqr)
        {
            return -diff;
        }
    }

    return aCurrentPosition;
}
