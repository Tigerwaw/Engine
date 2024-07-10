#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/BRDF_Functions.hlsli"
#include "Includes/Lambertian_Functions.hlsli"
#include "Includes/ConstantBuffers/FrameBuffer.hlsli"
#include "Includes/ConstantBuffers/LightBuffer.hlsli"
#include "Includes/ConstantBuffers/MaterialBuffer.hlsli"

TextureCube EnvCubeMap : register(t126);

Texture2D ShadowMapDir : register(t100);
TextureCube ShadowMapPoint[4] : register(t101);
Texture2D ShadowMapSpot[4] : register(t105);

float4 main(DefaultVStoPS input) : SV_TARGET
{
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
    
    
    // LIGHTING
    const float ambientOcclusion = materialMap.r;
    const float roughness = materialMap.g;
    const float metalness = materialMap.b;
    
    const float3 specularColor = lerp((float3) 0.04f, albedoMap.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedoMap.rgb, 1 - metalness);
    
    float3 ambientLight = BRDF_AmbientLighting(FB_ViewPosition.xyz, diffuseColor, specularColor, pixelNormal, input.WorldPos.xyz, roughness, ambientOcclusion, EnvCubeMap, LB_AmbientLight.Color, LB_AmbientLight.Intensity);
    float3 directionalLightRadiance = BRDF_DirectionalLight(LB_DirLight, FB_ViewPosition.xyz, input.WorldPos, pixelNormal, diffuseColor, specularColor, roughness, ShadowMapDir, 8);
    
    float3 pointLightRadiance = 0;
    [unroll(4)]
    for (int pIndex = 0; pIndex < LB_NumPointLights; pIndex++)
    {
        pointLightRadiance += BRDF_PointLight(LB_PointLights[pIndex], FB_ViewPosition.xyz, input.WorldPos, pixelNormal, diffuseColor, specularColor, roughness, ShadowMapPoint[pIndex], 8);
    }
    
    float3 spotLightRadiance = 0;
    [unroll(4)]
    for (int sIndex = 0; sIndex < LB_NumSpotLights; sIndex++)
    {
        spotLightRadiance += BRDF_SpotLight(LB_SpotLights[sIndex], FB_ViewPosition.xyz, input.WorldPos, pixelNormal, diffuseColor, specularColor, roughness, ShadowMapSpot[sIndex], 8);
    }
    
    float3 radiance = ambientLight + saturate(directionalLightRadiance + pointLightRadiance + spotLightRadiance);
    
    float4 color = 1;
    color.rgb = saturate(radiance);
    color.a = albedoMap.a;
    
    // DEBUG
    float3 debugInputNormals = (input.Normal + 1) * 0.5f;
    float3 debugInputTangents = (input.Tangent + 1) * 0.5f;
    float3 debugInputBinormals = (input.Binormal + 1) * 0.5f;
    float3 debugCalculatedNormals = (calculatedNormals + 1) * 0.5f;
    float3 debugPixelNormals = (pixelNormal + 1) * 0.5f;
    float3 debugUVs = float3(input.TexCoord0.x, input.TexCoord0.y, 0);
    float4 debug = float4(debugUVs, 1);
    
    return color;
}