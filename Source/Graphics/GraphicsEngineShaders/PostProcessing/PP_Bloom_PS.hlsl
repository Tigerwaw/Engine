#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/PostProcessing_Includes.hlsli"

Texture2D InputTexture : register(t30);
Texture2D IntermediateLDR : register(t31);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    const float3 sceneColor = IntermediateLDR.Sample(DefaultSampler, input.UV).rgb;
    const float3 bloomColor = InputTexture.Sample(DefaultSampler, input.UV).rgb;
    
    //return Bloom(sceneColor, bloomColor);
    return BloomScaledToScene(sceneColor, bloomColor);
    //return BloomScaledToLuminance(sceneColor, bloomColor);
}