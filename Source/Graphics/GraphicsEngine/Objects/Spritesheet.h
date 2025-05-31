#pragma once
#include "Sprite.h"
#include "Math/Vector.hpp"


class Spritesheet : public Sprite
{
public:
    void SetSheetDimensions(unsigned x, unsigned y);
    void SetCurrentFrame(unsigned aCurrentFrame);

    const Math::Vector2<unsigned> GetSheetDimensions() const { return myDimensions; }
    const unsigned GetCurrentFrame() const { return myCurrentFrame; }
private:
    Math::Vector2<unsigned> myDimensions;
    unsigned myCurrentFrame;
};

