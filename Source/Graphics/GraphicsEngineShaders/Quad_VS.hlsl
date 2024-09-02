#include "Includes/BRDF_Functions.hlsli"
#include "Includes/DefaultShaderIncludes.hlsli"

Quad_VS_to_PS main(unsigned int aVertexIndex : SV_VertexID)
{
    const float4 pos[4] =
    {
        float4(-1, -1, 0, 1),
        float4(-1, 1, 0, 1),
        float4(1, -1, 0, 1),
        float4(1, 1, 0, 1)
    };

    const float2 uv[4] =
    {
        float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
    };

    Quad_VS_to_PS returnValue;
    returnValue.Position = pos[aVertexIndex];
    returnValue.UV = uv[aVertexIndex];
    return returnValue;
}
