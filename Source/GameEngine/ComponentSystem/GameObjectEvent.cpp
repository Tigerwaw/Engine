#include "Enginepch.h"

#include "GameObjectEvent.h"

GameObjectEvent::GameObjectEvent(GameObjectEventType aType, std::any aData)
{
    type = aType;
    data = aData;
}
