#include "Includes/DefaultShaderIncludes.hlsli"

float4 main(Sprite_GSout input) : SV_TARGET
{
	return AlbedoTexture.Sample(DefaultSampler, input.TexCoord0.xy);
}