#include "ConstantBuffers/FrameBuffer.hlsli"
#include "DefaultShaderIncludes.hlsli"

#ifndef _DEFERRED_INCLUDES_
#define _DEFERRED_INCLUDES_

Texture2D GBuffer_Albedo : register(t0);
Texture2D GBuffer_Material : register(t1);
Texture2D GBuffer_Effects : register(t2);
Texture2D GBuffer_WorldNormal : register(t3);
Texture2D GBuffer_WorldPos : register(t4);

struct GBufferOutput
{
    float4 Albedo : SV_TARGET0;
    float4 Material : SV_TARGET1;
    float4 Effects : SV_TARGET2;
    float4 WorldNormal : SV_TARGET3;
    float4 WorldPosition : SV_TARGET4;
};

float4 GetViewSpacePosition(float2 UV)
{
    float4 position = GBuffer_WorldPos.SampleLevel(PointClampSampler, UV, 0);
    return mul(FB_InvView, position);
}

float4 GetViewSpaceNormal(float2 UV)
{
    float4 normal = GBuffer_WorldNormal.SampleLevel(PointClampSampler, UV, 0);
    return mul(FB_InvView, normal);
}

#endif // _DEFERRED_INCLUDES_