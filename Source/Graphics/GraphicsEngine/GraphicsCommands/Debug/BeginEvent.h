#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"

struct BeginEvent : public GraphicsCommandBase
{
public:
    BeginEvent(std::string_view aEventName);
    void Execute() override;
    void Destroy() override;
private:
    std::string_view myEventName;
};

