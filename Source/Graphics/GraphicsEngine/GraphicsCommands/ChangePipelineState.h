#pragma once
#include "GraphicsCommandBase.h"

struct PipelineStateObject;

struct ChangePipelineState : public GraphicsCommandBase
{
public:
    ChangePipelineState(std::shared_ptr<PipelineStateObject> aPipelineStateObject);
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<PipelineStateObject> myPipelineStateObject;
};