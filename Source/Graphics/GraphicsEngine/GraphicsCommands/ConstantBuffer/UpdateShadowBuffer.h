#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"
#include <memory>

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

