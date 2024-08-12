#pragma once
#include "GraphicsCommandBase.h"

class Texture;

struct SetTextureResource : public GraphicsCommandBase
{
public:
    SetTextureResource(unsigned aSlot, std::shared_ptr<Texture> aTexture);
    void Execute() override;
    void Destroy() override;
private:
    unsigned mySlot;
    std::shared_ptr<Texture> myTexture;
};

