#ifndef _PARTICLE_INCLUDES_
#define _PARTICLE_INCLUDES_

struct ParticleVertex
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float GravityScale : GRAVITYSCALE;
    float Lifetime : LIFETIME;
    float Angle : ANGLE;
    float2 Size : SIZE;
    float4 ChannelMask : CHANNELMASK;
};

struct Particle_VSOut
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float GravityScale : GRAVITYSCALE;
    float Lifetime : LIFETIME;
    float Angle : ANGLE;
    float2 Size : SIZE;
    float4 ChannelMask : CHANNELMASK;
};

struct Particle_GSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float GravityScale : GRAVITYSCALE;
    float Lifetime : LIFETIME;
    float4 ChannelMask : CHANNELMASK;
};

struct TrailVertex
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float Lifetime : LIFETIME;
    float Width : WIDTH;
    float4 ChannelMask : CHANNELMASK;
};

struct Trail_VSOut
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float Lifetime : LIFETIME;
    float Width : WIDTH;
    float4 ChannelMask : CHANNELMASK;
};

struct Trail_GSOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
    float Lifetime : LIFETIME;
    float4 ChannelMask : CHANNELMASK;
};

#endif // _DEFAULT_INCLUDES_