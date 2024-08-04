#include "GraphicsEngine.pch.h"
#include "Spritesheet.h"

void Spritesheet::SetSheetDimensions(unsigned x, unsigned y)
{
    myDimensions = { x, y };
}

void Spritesheet::SetCurrentFrame(unsigned aCurrentFrame)
{
    myCurrentFrame = aCurrentFrame;
}
