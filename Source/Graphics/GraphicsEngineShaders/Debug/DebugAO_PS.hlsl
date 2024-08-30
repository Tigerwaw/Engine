#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float4 ao = MaterialTexture.Sample(DefaultSampler, input.TexCoord0.xy).r;
    float4 color = saturate(ao);
    return color;
}