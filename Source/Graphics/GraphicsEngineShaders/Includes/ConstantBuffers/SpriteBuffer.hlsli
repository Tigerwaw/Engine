#ifndef _SPRITE_BUFFER_
#define _SPRITE_BUFFER_

cbuffer SpriteBuffer : register(b6)
{
    float4x4 SPB_Matrix;
    bool SPB_IsScreenSpace;
    
    // Spritesheet anim
    float SPB_CurrentFrame;
    float2 SPB_SpritesheetDimensions;
}

#endif // _SPRITE_BUFFER_