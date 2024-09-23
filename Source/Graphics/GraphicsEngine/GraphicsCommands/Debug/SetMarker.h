#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"

struct SetMarker : public GraphicsCommandBase
{
public:
    SetMarker(std::string_view aMarkerName);
    void Execute() override;
    void Destroy() override;
private:
    std::string_view myMarkerName;
};

