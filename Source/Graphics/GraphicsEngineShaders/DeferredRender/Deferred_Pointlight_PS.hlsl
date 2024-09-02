#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/BRDF_Lights.hlsli"

TextureCube ShadowMapPoint[4] : register(t101);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    float3 albedoColor = Def_Albedo.Sample(DefaultSampler, input.UV).rgb;
    float3 material = Def_Material.Sample(DefaultSampler, input.UV).rgb;
    float3 worldNormal = Def_WorldNormal.Sample(DefaultSampler, input.UV).rgb;
    float4 worldPos = Def_WorldPos.Sample(DefaultSampler, input.UV);
    
    const float ambientOcclusion = material.r;
    const float roughness = material.g;
    const float metalness = material.b;

    const float3 specularColor = lerp((float3) 0.04f, albedoColor.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedoColor.rgb, 1 - metalness);
    
    float3 radiance = 0;
    [unroll(4)]
    for (int pIndex = 0; pIndex < LB_NumPointLights; pIndex++)
    {
        radiance += BRDF_PointLight(LB_PointLights[pIndex], FB_ViewPosition.xyz, worldPos, worldNormal, diffuseColor, specularColor, roughness, ShadowMapPoint[pIndex]);
    }
    
    return float4(radiance, 1.0f);
}