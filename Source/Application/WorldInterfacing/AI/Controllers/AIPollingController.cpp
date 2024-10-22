#include "Enginepch.h"
#include "AIPollingController.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "WorldInterfacing/AI/PollingStation.h"

void AIPollingController::Start()
{
}

CU::Vector3f AIPollingController::GetDirection(CU::Vector3f aCurrentPosition)
{
    PollingStation& ps = PollingStation::Get();
    if (!ps.IsPlayerHackingComputer()) return CU::Vector3f();

    CU::Vector3f diff = ps.GetPlayerPosition() - aCurrentPosition;
    if (diff.LengthSqr() < 1.0f) return CU::Vector3f();

    return diff;
}
