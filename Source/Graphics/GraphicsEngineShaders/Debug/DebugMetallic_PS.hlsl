#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float4 metallic = MaterialTexture.Sample(LinearWrapSampler, input.TexCoord0.xy).b;
    float4 color = saturate(metallic);
    return color;
}