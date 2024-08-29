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

struct TextVertex
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct Text_VSout
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

#endif // _SPRITE_INCLUDES_