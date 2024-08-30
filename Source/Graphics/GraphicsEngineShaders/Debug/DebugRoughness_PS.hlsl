#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float4 roughness = MaterialTexture.Sample(DefaultSampler, input.TexCoord0.xy).g;
    float4 color = saturate(roughness);
    return color;
}