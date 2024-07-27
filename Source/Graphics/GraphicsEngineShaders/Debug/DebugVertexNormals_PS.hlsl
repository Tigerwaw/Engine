#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float3 debugNormals = (input.Normal + 1) * 0.5;

    return float4(debugNormals, 1);
}