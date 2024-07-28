#include "GraphicsEngine.pch.h"
#include "ChangePipelineState.h"

ChangePipelineState::ChangePipelineState(std::shared_ptr<PipelineStateObject> aPipelineStateObject)
{
    myPipelineStateObject = aPipelineStateObject;
}

void ChangePipelineState::Execute()
{
    GraphicsEngine::Get().ChangePipelineState(myPipelineStateObject);
}

void ChangePipelineState::Destroy()
{
    myPipelineStateObject = nullptr;
}
