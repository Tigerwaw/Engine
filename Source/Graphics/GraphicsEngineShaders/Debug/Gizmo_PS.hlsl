#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(DefaultVStoPS input) : SV_TARGET
{
    return input.VertexColor0;
}