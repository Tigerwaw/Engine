#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    float3 debugBinormals = (input.Binormal + 1) * 0.5;

    return float4(debugBinormals, 1);
}