#include "../../Includes/Debug_Includes.hlsli"
#include "../../Includes/ConstantBuffers/DebugBuffer.hlsli"
#include "../../Includes/ConstantBuffers/FrameBuffer.hlsli"

[maxvertexcount(2)]
void main(point DebugObject_VSout input[1],
		  inout LineStream<DebugObject_GSout> output)
{
    uint vertexID = input[0].vertexID;
    float4 pos[2] = { DB_Lines[vertexID].From, DB_Lines[vertexID].To };

	for (uint i = 0; i < 2; i++)
	{
		DebugObject_GSout element;
		float4 viewPos = mul(FB_InvView, pos[i]);
		float4 screenPos = mul(FB_Projection, viewPos);
        element.Position = screenPos;
		element.Color = DB_Lines[vertexID].Color;
		output.Append(element);
	}
}