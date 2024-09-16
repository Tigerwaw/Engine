#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/PostProcessing_Includes.hlsli"

Texture2D IntermediateLDRBuffer : register(t30);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    const float4 color = IntermediateLDRBuffer.Sample(DefaultSampler, input.UV);
    
    if (color.a < 0.05f)
        discard;
    
    //return LuminanceRandomGain(color.rgb, 5);
    //return LuminanceContrast(color.rgb, 0.8f);
    //return LuminanceReductionAndGain(color.rgb, 0.8f);
    return LuminanceFade(color.rgb, 0.8f, 5);
}