#pragma once
#include "GameObjectEventType.h"

struct GameObjectEvent
{
    GameObjectEventType type;
    std::any data;

    GameObjectEvent(GameObjectEventType aType, std::any aData);
};