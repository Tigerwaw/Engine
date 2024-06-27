cbuffer FrameBuffer : register(b0)
{
    float4x4 FB_InvView;
    float4x4 FB_Projection;
    float4 FB_ViewPosition;
    float4 FB_ViewDirection;
    float NearPlane;
    float FarPlane;
    float2 FB_Padding;
    float2 FB_Time;
    float2 FB_Resolution;
}