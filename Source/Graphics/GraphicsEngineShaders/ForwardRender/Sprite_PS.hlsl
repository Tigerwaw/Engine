#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Sprite_Includes.hlsli"

float4 main(Sprite_GSout input) : SV_TARGET
{
	return AlbedoTexture.Sample(LinearWrapSampler, input.TexCoord0.xy);
}