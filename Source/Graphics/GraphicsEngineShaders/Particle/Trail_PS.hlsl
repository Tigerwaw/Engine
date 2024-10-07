#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ParticleIncludes.hlsli"

Texture2D TrailTexture : register(t0);

float4 main(Trail_GSOut input) : SV_TARGET
{    
    if (input.Lifetime < 0.0f)
    {
        discard;
        return 0;
    }
    
    const float4 textureColor = TrailTexture.Sample(LinearWrapSampler, input.UV) * input.ChannelMask;
    const float color = textureColor.r + textureColor.g + textureColor.b + textureColor.a;
    return color * input.Color;
}