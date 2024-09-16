#ifndef _PP_FUNCS_
#define _PP_FUNCS_


// Luminance

static const float3 kernel = float3(0.2126f, 0.7152f, 0.0722f);

float4 LuminanceRandomGain(float3 aColor, float aPower)
{
    float luminance = dot(aColor, kernel);
    float3 result = aColor * pow(luminance, aPower);
    return float4(result, 1);
}

float4 LuminanceContrast(float3 aColor, float aThreshold)
{
    float luminance = dot(aColor, kernel);
    float threshold = aThreshold;
    
    [flatten]
    if (luminance >= threshold)
    {
        return float4(aColor, 1);
    }
    else
    {
        return 0;
    }
}

float4 LuminanceReductionAndGain(float3 aColor, float aThreshold)
{
    float threshold = aThreshold;
    float luminance = saturate(dot(aColor, kernel) - threshold);
    return float4(aColor * luminance * (1.0f / threshold), 1);
}

float4 LuminanceFade(float3 aColor, float aThreshold, float aPower)
{
    float luminance = dot(aColor, kernel);
    float threshold = aThreshold;
    
    [flatten]
    if (luminance >= threshold)
    {
        return float4(aColor, 1);
    }
    else if (luminance >= threshold * 0.5f)
    {
        float fade = luminance / threshold;
        fade = pow(fade, aPower);
        return float4(aColor * fade, 1);
    }
    else
    {
        return 0;
    }
}


// Bloom

float4 BloomAdditive(float3 aSceneColor, float3 aBloomColor)
{
    return float4(aSceneColor + aBloomColor, 1);
}

float4 BloomScaledToScene(float3 aSceneColor, float3 aBloomColor)
{
    return float4(aSceneColor + (aBloomColor * (1.0f - aSceneColor)), 1);
}

float4 BloomScaledToLuminance(float3 aSceneColor, float3 aBloomColor)
{
    const float luminance = dot(aSceneColor, float3(0.2126f, 0.7152f, 0.0722f));
    return float4(aSceneColor + (aBloomColor * (1.0f - luminance)), 1);
}

#endif // _PP_FUNCS_