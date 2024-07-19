#include "../../Includes/Debug_Includes.hlsli"
#include "../../Includes/ConstantBuffers/FrameBuffer.hlsli"

DebugLine_VSout main(DebugLineVertex vertex)
{
    DebugLine_VSout result;
    float4 viewPosFrom = mul(FB_InvView, vertex.From);
	float4 screenPosFrom = mul(FB_Projection, viewPosFrom);
    result.From = screenPosFrom;
    
    float4 viewPosTo = mul(FB_InvView, vertex.To);
	float4 screenPosTo = mul(FB_Projection, viewPosTo);
    result.To = screenPosTo;
    
    result.Color = vertex.Color;
   
	return result;
}