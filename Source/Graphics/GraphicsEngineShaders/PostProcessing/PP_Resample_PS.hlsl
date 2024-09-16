#include "../Includes/DefaultShaderIncludes.hlsli"

Texture2D InputTexture : register(t30);

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    return InputTexture.Sample(DefaultClampSampler, input.UV);

}