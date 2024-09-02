#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"
#include "../Includes/ConstantBuffers/ShadowBuffer.hlsli"

[maxvertexcount(18)]
void main(triangle ShadowCube_VSout input[3], inout TriangleStream<ShadowCube_GSout> output)
{
    for (int f = 0; f < 6; f++)
    {
        ShadowCube_GSout Out;
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