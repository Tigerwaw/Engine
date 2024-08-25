#include "ConstantBuffers/LightBuffer.hlsli"
#include "BRDF_Functions.hlsli"
#include "BRDF_Shadows.hlsli"

#ifndef _BRDF_LIGHTS_
#define _BRDF_LIGHTS_

// Calculating lighting from different types of light sources

float3 BRDF_DirectionalLight(DirLightData dirLight, float3 viewPos, float4 worldPos, float3 pixelNormal, float3 diffuseColor, float3 specularColor, float roughness, Texture2D shadowMap)
{
    float3 directLight = BRDF_DirectLighting(viewPos, worldPos.xyz, dirLight.Color, -dirLight.Direction, worldPos.xyz, pixelNormal, diffuseColor, specularColor, roughness);

    // Attenuation
    float lightAttenuation = saturate(dot(pixelNormal, -dirLight.Direction));
    float intensity = dirLight.Intensity * lightAttenuation;
    directLight *= intensity;
    
    float shadowFactor = ShadowFactorDLightSpotLight(dirLight.View, dirLight.Projection, dirLight.Direction, worldPos, pixelNormal, dirLight.MinBias, dirLight.MaxBias, shadowMap, dirLight.LightSize, dirLight.FrustumSize.x, dirLight.NearPlane);
    directLight *= saturate(shadowFactor);
    return directLight;
}

float3 BRDF_PointLight(PointLightData pointLight, float3 viewPos, float4 worldPos, float3 pixelNormal, float3 diffuseColor, float3 specularColor, float roughness, TextureCube shadowCubemap)
{
    float3 L = pointLight.Position - worldPos.xyz;
    float3 directLight = BRDF_DirectLighting(viewPos, pointLight.Position, pointLight.Color, L, worldPos.xyz, pixelNormal, diffuseColor, specularColor, roughness);
            
    // Attenuation
    float d = length(L);
    float lightAttenuation = (1 / (d * d));
    float intensity = pointLight.Intensity * lightAttenuation;
    directLight *= intensity;
    
    float shadowFactor = ShadowFactorPointLight(pointLight, worldPos, pixelNormal, pointLight.MinBias, pointLight.MaxBias, shadowCubemap, pointLight.LightSize, pointLight.FrustumSize.x, pointLight.NearPlane);
    directLight *= saturate(shadowFactor);
    return directLight;
}

float3 BRDF_SpotLight(SpotLightData spotLight, float3 viewPos, float4 worldPos, float3 pixelNormal, float3 diffuseColor, float3 specularColor, float roughness, Texture2D shadowMap)
{
    float3 L = spotLight.Position - worldPos.xyz;
    float3 directLight = BRDF_DirectLighting(viewPos, spotLight.Position, spotLight.Color, L, worldPos.xyz, pixelNormal, diffuseColor, specularColor, roughness);
            
    // Attenuation
    float spotCone = pow(saturate(dot(normalize(L), normalize(-spotLight.Direction))), spotLight.ConeAngle);
    float d = length(L);
    float lightAttenuation = (1 / (d * d) * spotCone);
    float intensity = spotLight.Intensity * lightAttenuation;
    directLight *= intensity;
            
    float shadowFactor = ShadowFactorDLightSpotLight(spotLight.View, spotLight.Projection, spotLight.Direction, worldPos, pixelNormal, spotLight.MinBias, spotLight.MaxBias, shadowMap, spotLight.LightSize, spotLight.FrustumSize.x, spotLight.NearPlane);
    directLight *= saturate(shadowFactor);
    return directLight;
}

#endif