#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/BRDF_Lights.hlsli"

TextureCube EnvCubeMap : register(t126);
Texture2D ShadowMapDir : register(t100);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    float3 albedoColor = Def_Albedo.Sample(DefaultSampler, input.UV).rgb;
    float3 material = Def_Material.Sample(DefaultSampler, input.UV).rgb;
    float4 effects = Def_Effects.Sample(DefaultSampler, input.UV);
    float3 worldNormal = Def_WorldNormal.Sample(DefaultSampler, input.UV).rgb;
    float4 worldPos = Def_WorldPos.Sample(DefaultSampler, input.UV);
    
    const float ambientOcclusion = material.r;
    const float roughness = material.g;
    const float metalness = material.b;
    
    const float3 emission = albedoColor * effects.r;

    const float3 specularColor = lerp((float3) 0.04f, albedoColor.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedoColor.rgb, 1 - metalness);
    
    float3 ambientLight = BRDF_AmbientLighting(FB_ViewPosition.xyz, diffuseColor, specularColor, worldNormal, worldPos.rgb, roughness, ambientOcclusion, EnvCubeMap, LB_AmbientLight.Color, LB_AmbientLight.Intensity);
    float3 directionalLightRadiance = BRDF_DirectionalLight(LB_DirLight, FB_ViewPosition.xyz, worldPos, worldNormal, diffuseColor, specularColor, roughness, ShadowMapDir);
    
    float3 radiance = ambientLight + directionalLightRadiance + emission;
    
    return float4(radiance, 1.0f);
}