#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/ObjectBuffer.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"

Particle_VSOut main(ParticleVertex input)
{
    Particle_VSOut output;
    
    float4 worldPos = mul(OB_World, input.Position);
    float4 viewPos = mul(FB_InvView, worldPos);
    output.Position = viewPos;
    output.Color = input.Color;
    output.Velocity = input.Velocity;
    output.GravityScale = input.GravityScale;
    output.Lifetime = input.Lifetime;
    output.Angle = input.Angle;
    output.Size = input.Size;
    output.ChannelMask = input.ChannelMask;
    
    return output;
}