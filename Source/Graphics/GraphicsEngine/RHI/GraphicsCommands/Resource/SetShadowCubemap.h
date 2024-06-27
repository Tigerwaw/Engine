#pragma once
#include "GraphicsCommandBase.h"
#include <memory>

class Texture;

struct SetShadowCubemap : public GraphicsCommandBase
{
public:
    SetShadowCubemap(std::shared_ptr<Texture> aShadowMap);
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<Texture> myShadowCubemap;
};

