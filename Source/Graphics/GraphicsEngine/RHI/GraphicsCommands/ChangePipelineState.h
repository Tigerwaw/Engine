#pragma once
#include "GraphicsCommandBase.h"
#include "Graphics/GraphicsEngine/PipelineStateType.h"

struct ChangePipelineState : public GraphicsCommandBase
{
public:
    ChangePipelineState(PipelineStateType aPipelineStateType);
    void Execute() override;
    void Destroy() override;
private:
    PipelineStateType myPipelineStateType;
};