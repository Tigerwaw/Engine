#ifndef _FRAME_BUFFER_
#define _FRAME_BUFFER_

cbuffer FrameBuffer : register(b0)
{
    float4x4 FB_InvView;
    float4x4 FB_Projection;
    float4 FB_ViewPosition;
    float4 FB_ViewDirection;
    float FB_NearPlane;
    float FB_FarPlane;
    int FB_DebugRenderMode;
    float FB_Padding;
    float2 FB_Time; // x = TotalTime, y = Delta Time
    float2 FB_Resolution;
}

#endif // _FRAME_BUFFER_