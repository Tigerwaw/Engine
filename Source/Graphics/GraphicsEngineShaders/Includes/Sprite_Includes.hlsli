#ifndef _SPRITE_INCLUDES_
#define _SPRITE_INCLUDES_

struct Sprite_VSout
{
    float4x4 LocalSpaceMatrix   : POSITION;
};

struct Sprite_GSout
{
    float4 Position     : SV_Position;
    float2 TexCoord0    : TEXCOORD0;
    uint PrimID         : SV_PrimitiveID;
};

#endif // _SPRITE_INCLUDES_