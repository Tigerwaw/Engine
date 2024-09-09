#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Tonemap.hlsli"

Texture2D IntermediateBuffer : register(t10);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    return float4(LinearToGamma(Tonemap_ACES(IntermediateBuffer.Sample(DefaultSampler, input.UV).rgb)), 1.0f);
}