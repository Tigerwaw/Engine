#include "GraphicsEngine.pch.h"
#include "ChangePipelineState.h"

ChangePipelineState::ChangePipelineState(PipelineStateType aPipelineStateType)
{
    myPipelineStateType = aPipelineStateType;
}

void ChangePipelineState::Execute()
{
    GraphicsEngine::Get().ChangePipelineState(myPipelineStateType);
}

void ChangePipelineState::Destroy()
{
}
