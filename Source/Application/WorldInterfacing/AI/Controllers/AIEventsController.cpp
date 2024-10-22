#include "Enginepch.h"
#include "AIEventsController.h"
#include "WorldInterfacing/AI/AIEventManager.h"

AIEventsController::~AIEventsController()
{
    AIEventManager::Get().Unsubscribe("PlayerIsHacking", this);
}

void AIEventsController::Start()
{
    AIEventManager::Get().Subscribe("PlayerIsHacking", this);
}

CU::Vector3f AIEventsController::GetDirection(CU::Vector3f aCurrentPosition)
{
    if (!myIsPlayerHacking) return CU::Vector3f();

    CU::Vector3f diff = myPlayerPosition - aCurrentPosition;

    if (diff.LengthSqr() < 1.0f) return CU::Vector3f();

    return diff;
}

void AIEventsController::Receive(const AIEvent& aEvent)
{
    if (aEvent.type == "PlayerIsHacking")
    {
        if (aEvent.data.has_value())
        {
            myIsPlayerHacking = true;
            myPlayerPosition = std::any_cast<CU::Vector3f>(aEvent.data);
        }
    }
}
