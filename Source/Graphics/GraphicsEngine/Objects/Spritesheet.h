#pragma once
#include "Sprite.h"
#include "GameEngine\Math\Vector.hpp"
namespace CU = CommonUtilities;

class Spritesheet : public Sprite
{
public:
    void SetSheetDimensions(unsigned x, unsigned y);
    void SetCurrentFrame(unsigned aCurrentFrame);

    const CU::Vector2<unsigned> GetSheetDimensions() const { return myDimensions; }
    const unsigned GetCurrentFrame() const { return myCurrentFrame; }
private:
    CU::Vector2<unsigned> myDimensions;
    unsigned myCurrentFrame;
};

