#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class SetMarker : public GraphicsCommandBase
{
public:
    SetMarker(std::string_view aMarkerName);
    void Execute() override;
    void Destroy() override;
private:
    std::string_view myMarkerName;
};

