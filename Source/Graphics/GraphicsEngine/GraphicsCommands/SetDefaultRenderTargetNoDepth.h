#pragma once
#include "GraphicsCommandBase.h"

class Texture;

class SetDefaultRenderTargetNoDepth : public GraphicsCommandBase
{
public:
    SetDefaultRenderTargetNoDepth(bool clearRTs = false);
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<Texture> myRenderTarget;
    bool myClearRenderTarget;
    bool myClearDepthStencil;
};

