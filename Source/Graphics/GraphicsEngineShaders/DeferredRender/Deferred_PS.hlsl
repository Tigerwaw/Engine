#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/MaterialBuffer.hlsli"

GBufferOutput main(MeshVStoPS input)
{
    GBufferOutput output;
    
    const float4 albedoMap = AlbedoTexture.Sample(DefaultSampler, input.TexCoord0.xy);
    const float2 normalMap = NormalTexture.Sample(DefaultSampler, input.TexCoord0.xy).rg;
    const float3 materialMap = MaterialTexture.Sample(DefaultSampler, input.TexCoord0.xy).rgb;
    
    if (albedoMap.a < 0.01)
    {
        discard;
    }
   
    // NORMALS
    float3 calculatedNormals;
    calculatedNormals.xy = (normalMap.xy - 0.5f) * 2;
    calculatedNormals.z = sqrt(1 - saturate(calculatedNormals.x * calculatedNormals.x + calculatedNormals.y * calculatedNormals.y));
    calculatedNormals = normalize(calculatedNormals);
    
    const float3x3 TBN = float3x3(
        normalize(input.Tangent),
        normalize(input.Binormal),
        normalize(input.Normal)
    );
    
    float3 pixelNormal = normalize(mul(calculatedNormals, TBN));
    
    output.Albedo = albedoMap * MB_AlbedoTint;
    output.Material = float4(materialMap.rgb, 0);
    output.WorldNormal = float4(pixelNormal.rgb, 0);
    output.WorldPosition = input.WorldPos;
    
    return output;
}