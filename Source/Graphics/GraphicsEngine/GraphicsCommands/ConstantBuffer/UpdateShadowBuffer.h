#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class PointLight;

struct UpdateShadowBuffer : public GraphicsCommandBase
{
public:
    UpdateShadowBuffer(std::shared_ptr<PointLight> aPointLight);
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<PointLight> myPointLight;
};

