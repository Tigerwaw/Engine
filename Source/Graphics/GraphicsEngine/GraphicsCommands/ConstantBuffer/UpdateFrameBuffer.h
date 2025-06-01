#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Objects/ConstantBuffers/FrameBuffer.h"

class UpdateFrameBuffer : public GraphicsCommandBase
{
public:
    UpdateFrameBuffer(const FrameBuffer& aFrameBuffer);
    void Execute() override;
    void Destroy() override;
private:
    FrameBuffer myFrameBuffer;
};

