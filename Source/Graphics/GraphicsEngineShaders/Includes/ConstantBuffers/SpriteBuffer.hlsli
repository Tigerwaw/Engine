#ifndef _SPRITE_BUFFER_
#define _SPRITE_BUFFER_

cbuffer SpriteBuffer : register(b6)
{
    float4 SPB_Position;
    float2 SPB_Size;
    bool SPB_IsScreenSpace;
    float SPB_Padding;
}

#endif // _SPRITE_BUFFER_