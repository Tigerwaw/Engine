#include "GraphicsEngine.pch.h"
#include "SetRenderTarget.h"

SetRenderTarget::SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
    myRenderTarget = aRenderTarget;
    myDepthStencil = aDepthStencil;
    myClearRenderTarget = aClearRenderTarget;
    myClearDepthStencil = aClearDepthStencil;
}

void SetRenderTarget::Execute()
{
    GraphicsEngine::Get().SetRenderTarget(myRenderTarget, myDepthStencil, myClearRenderTarget, myClearDepthStencil);
}

void SetRenderTarget::Destroy()
{
    myRenderTarget = nullptr;
    myDepthStencil = nullptr;
}
