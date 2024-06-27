#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(DefaultVStoPS input) : SV_TARGET
{
    const float2 normalMap = NormalTexture.Sample(DefaultSampler, input.TexCoord0.xy).rg;
    
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
    
    pixelNormal = (pixelNormal + 1) * 0.5;
    
    return float4(pixelNormal, 1);
}