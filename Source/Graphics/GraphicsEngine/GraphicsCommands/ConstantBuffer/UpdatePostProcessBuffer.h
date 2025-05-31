#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Objects/ConstantBuffers/PostProcessBuffer.h"

struct UpdatePostProcessBuffer : public GraphicsCommandBase
{
public:
    UpdatePostProcessBuffer();
    void Execute() override;
    void Destroy() override;
};

