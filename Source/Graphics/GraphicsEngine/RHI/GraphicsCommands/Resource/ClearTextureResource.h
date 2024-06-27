#pragma once
#include "GraphicsCommandBase.h"

struct ClearTextureResource : public GraphicsCommandBase
{
public:
    ClearTextureResource(unsigned aSlot);
    void Execute() override;
    void Destroy() override;
private:
    unsigned mySlot;
};

