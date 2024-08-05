#ifndef _SPRITE_BUFFER_
#define _SPRITE_BUFFER_

cbuffer SpriteBuffer : register(b6)
{
    float4x4 SPB_Matrix;
    float SPB_CurrentFrame;
    float2 SPB_SpritesheetDimensions;
    float SPB_Padding;
}

#endif // _SPRITE_BUFFER_