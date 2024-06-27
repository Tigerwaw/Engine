#include "Includes/DefaultShaderIncludes.hlsli"

[maxvertexcount(18)]
void main(triangle VS_to_GS input[3], inout TriangleStream<GS_OUT> output)
{
    for (int f = 0; f < 6; f++)
    {
        GS_OUT Out;
        Out.RTIndex = f;
        for (int v = 0; v < 3; v++)
        {
            float4 worldPos = input[v].Position;
            float4 lightSpacePos = mul(SB_CameraTransforms[f], worldPos);
            float4 lightProjPos = mul(FB_Projection, lightSpacePos);
            Out.Position = lightProjPos;
            output.Append(Out);
        }
        output.RestartStrip();
    }
}