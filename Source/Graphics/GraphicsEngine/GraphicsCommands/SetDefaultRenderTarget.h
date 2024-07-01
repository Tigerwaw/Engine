#pragma once
#include "GraphicsCommandBase.h"
#include <memory>

class Texture;

class SetDefaultRenderTarget : public GraphicsCommandBase
{
public:
    SetDefaultRenderTarget();
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<Texture> myRenderTarget;
    std::shared_ptr<Texture> myDepthStencil;
    bool myClearRenderTarget;
    bool myClearDepthStencil;
};

