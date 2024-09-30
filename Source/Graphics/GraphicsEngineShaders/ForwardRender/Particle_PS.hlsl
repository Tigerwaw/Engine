#include "../Includes/DefaultShaderIncludes.hlsli"

Texture2D ParticleTexture : register(t0);

float4 main(Particle_GSOut input) : SV_TARGET
{
	if (input.Lifetime < 0.0f)
    {
        discard;
        return 0;
    }
    
    const float4 textureColor = ParticleTexture.Sample(LinearWrapSampler, input.UV) * input.ChannelMask;
    const float color = textureColor.r + textureColor.g + textureColor.b + textureColor.a;
    return color * input.Color;
}