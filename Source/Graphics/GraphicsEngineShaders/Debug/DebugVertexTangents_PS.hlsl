#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float3 debugTangents = (input.Tangent + 1) * 0.5;

    return float4(debugTangents, 1);
}