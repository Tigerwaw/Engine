#pragma once
#include "GraphicsCommandBase.h"
#include <memory>

class Texture;

struct SetRenderTarget : public GraphicsCommandBase
{
public:
    SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<Texture> myRenderTarget;
    std::shared_ptr<Texture> myDepthStencil;
    bool myClearRenderTarget;
    bool myClearDepthStencil;
};

