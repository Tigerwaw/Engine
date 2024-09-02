#include "GraphicsEngine.pch.h"
#include "SetDefaultRenderTargetNoDepth.h"

SetDefaultRenderTargetNoDepth::SetDefaultRenderTargetNoDepth(bool clearRTs)
{
    myRenderTarget = GraphicsEngine::Get().GetBackBuffer();
    myClearRenderTarget = clearRTs;
    myClearDepthStencil = false;
}

void SetDefaultRenderTargetNoDepth::Execute()
{
    GraphicsEngine::Get().SetRenderTarget(myRenderTarget, nullptr, myClearRenderTarget, myClearDepthStencil);
}

void SetDefaultRenderTargetNoDepth::Destroy()
{
    myRenderTarget = nullptr;
}
