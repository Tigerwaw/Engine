#include "Enginepch.h"
#include "Wander.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "WorldInterfacing/AI/PollingStation.h"

void Wander::Start()
{
}

CU::Vector3f Wander::GetDirection(CU::Vector3f aCurrentPosition)
{
    CU::Vector3f dir = aCurrentPosition + aCurrentPosition.GetNormalized() * 100.0f - aCurrentPosition;
    if (dir.LengthSqr() < 1.0f) return CU::Vector3f();

    return dir;
}
