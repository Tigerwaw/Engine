#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/MaterialBuffer.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float4 albedo = AlbedoTexture.Sample(LinearWrapSampler, input.TexCoord0.xy);
    float4 color = saturate(albedo) * saturate(MB_AlbedoTint);
    return color;
}