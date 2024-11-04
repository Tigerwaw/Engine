#include "Enginepch.h"
#include "Seek.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "WorldInterfacing/AI/PollingStation.h"

void Seek::Start()
{
}

CU::Vector3f Seek::GetDirection(CU::Vector3f aCurrentPosition)
{
    CU::Vector3f diff = PollingStation::Get().GetWandererPosition() - aCurrentPosition;
    if (diff.LengthSqr() < 1.0f) return CU::Vector3f();

    return diff;
}
