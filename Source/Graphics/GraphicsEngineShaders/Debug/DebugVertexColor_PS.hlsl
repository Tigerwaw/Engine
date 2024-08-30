#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(MeshVStoPS input) : SV_TARGET
{
    return float4(input.VertexColor0.x, input.VertexColor0.y, 0, 1);
}