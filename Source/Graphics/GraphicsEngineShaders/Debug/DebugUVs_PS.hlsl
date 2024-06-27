#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(DefaultVStoPS input) : SV_TARGET
{
    return float4(input.TexCoord0.x, input.TexCoord0.y, 0, 1);
}