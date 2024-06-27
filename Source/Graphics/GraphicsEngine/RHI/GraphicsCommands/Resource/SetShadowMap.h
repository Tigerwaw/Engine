#pragma once
#include "GraphicsCommandBase.h"
#include <memory>

class Texture;

struct SetShadowMap : public GraphicsCommandBase
{
public:
    SetShadowMap(std::shared_ptr<Texture> aShadowMap);
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<Texture> myShadowMap;
};

