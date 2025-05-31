#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

struct EndEvent : public GraphicsCommandBase
{
public:
    EndEvent();
    void Execute() override;
    void Destroy() override;
};

