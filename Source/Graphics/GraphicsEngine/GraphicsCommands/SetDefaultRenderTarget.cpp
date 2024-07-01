#include "GraphicsEngine.pch.h"
#include "SetDefaultRenderTarget.h"

SetDefaultRenderTarget::SetDefaultRenderTarget()
{
    myRenderTarget = GraphicsEngine::Get().GetBackBuffer();
    myDepthStencil = GraphicsEngine::Get().GetDepthBuffer();
    myClearRenderTarget = true;
    myClearDepthStencil = true;
}

void SetDefaultRenderTarget::Execute()
{
    GraphicsEngine::Get().SetRenderTarget(myRenderTarget, myDepthStencil, myClearRenderTarget, myClearDepthStencil);
}

void SetDefaultRenderTarget::Destroy()
{
    myRenderTarget = nullptr;
    myDepthStencil = nullptr;
}
