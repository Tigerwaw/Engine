#ifndef _SPRITE_INCLUDES_
#define _SPRITE_INCLUDES_

struct Sprite_VSout
{
    float4 Position     : POSITION;
    float2 Size         : SIZE;
};

struct Sprite_GSout
{
    float4 Position     : SV_Position;
    float4 WorldPos     : WORLDPOSITION;
    float2 TexCoord0    : TEXCOORD0;
    uint PrimID         : SV_PrimitiveID;
};

#endif // _SPRITE_INCLUDES_