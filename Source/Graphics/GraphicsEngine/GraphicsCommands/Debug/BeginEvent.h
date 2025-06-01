#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class BeginEvent : public GraphicsCommandBase
{
public:
    BeginEvent(std::string_view aEventName);
    void Execute() override;
    void Destroy() override;
private:
    std::string_view myEventName;
};

