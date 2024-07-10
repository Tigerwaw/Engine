#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef _LAMBERTIAN_FUNCS_
#define _LAMBERTIAN_FUNCS_

float3 Lambertian_Light(float3 aLightDirection, float3 aPixelNormal, float3 aLightColor, float aLightIntensity)
{
    return dot(aLightDirection, aPixelNormal) * aLightColor * aLightIntensity;
}

float3 Lambertian_DirectionalLight(float3 aLightDirection, float3 aPixelNormal, float3 aLightColor, float aLightIntensity)
{
    return Lambertian_Light(normalize(-aLightDirection), aPixelNormal, aLightColor, aLightIntensity);
}

float3 Lambertian_PointLight(float3 L, float3 aPixelNormal, float3 aLightColor, float aLightIntensity)
{
    float3 light = saturate(dot(normalize(L), aPixelNormal)) * aLightColor * aLightIntensity;
    float d = length(L);
    float lightAttenuation = (1 / (d * d));
    light *= lightAttenuation;
    return saturate(light);
}

float3 Lambertian_Spotlight(float3 L, float3 aPixelNormal, float3 aLightColor, float aLightIntensity, float3 aLightDir, float aConeAngle)
{
    float3 light = saturate(dot(normalize(L), aPixelNormal)) * aLightColor * aLightIntensity;
    float d = length(L);
    float spotCone = pow(saturate(dot(normalize(L), normalize(-aLightDir))), PI - aConeAngle);
    float lightAttenuation = (1 / (d * d));
    light *= lightAttenuation;
    light *= saturate(spotCone);
    return saturate(light);
}

#endif // _LAMBERTIAN_FUNCS_