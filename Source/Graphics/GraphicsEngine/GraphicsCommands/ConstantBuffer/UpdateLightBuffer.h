#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Objects/ConstantBuffers/LightBuffer.h"

class UpdateLightBuffer : public GraphicsCommandBase
{
public:
    UpdateLightBuffer(const LightBuffer& aLightBuffer);
    void Execute() override;
    void Destroy() override;
private:
    LightBuffer myLightBuffer;
};

