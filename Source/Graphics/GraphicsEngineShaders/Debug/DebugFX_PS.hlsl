#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float4 fx = EffectsTexture.Sample(LinearWrapSampler, input.TexCoord0.xy);
    float4 color = saturate(fx);
    return color;
}