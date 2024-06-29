#ifndef _DEFAULT_INCLUDES_
#define _DEFAULT_INCLUDES_

#include "FrameBuffer.hlsli"
#include "ObjectBuffer.hlsli"
#include "AnimationBuffer.hlsli"
#include "MaterialBuffer.hlsli"
#include "LightBuffer.hlsli"
#include "ShadowBuffer.hlsli"
#include "SpriteBuffer.hlsli"

Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MaterialTexture : register(t2);

SamplerState DefaultSampler : register(s0);
SamplerComparisonState ShadowCmpSampler : register(s15);

struct DefaultVertex
{
    float4 Position     : POSITION;
    float4 VertexColor0 : VERTEXCOLOR0;
    float4 VertexColor1 : VERTEXCOLOR1;
    float4 VertexColor2 : VERTEXCOLOR2;
    float4 VertexColor3 : VERTEXCOLOR3;
    float2 TexCoord0    : TEXCOORD0;
    float2 TexCoord1    : TEXCOORD1;
    float2 TexCoord2    : TEXCOORD2;
    float2 TexCoord3    : TEXCOORD3;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
    uint4 BoneIDs       : BONEIDS;
    float4 Skinweights  : SKINWEIGHTS;
};

struct DefaultVStoPS
{
    float4 Position     : SV_POSITION;
    float4 WorldPos     : WORLDPOSITION;
    float4 ViewPos      : VIEWPOSITION;
    float4 VertexColor0 : VERTEXCOLOR0;
    float4 VertexColor1 : VERTEXCOLOR1;
    float4 VertexColor2 : VERTEXCOLOR2;
    float4 VertexColor3 : VERTEXCOLOR3;
    float2 TexCoord0    : TEXCOORD0;
    float2 TexCoord1    : TEXCOORD1;
    float2 TexCoord2    : TEXCOORD2;
    float2 TexCoord3    : TEXCOORD3;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
    float3 Binormal     : BINORMAL;
};

struct VS_to_GS
{
    float4 Position     : POSITION;
};

struct GS_OUT
{
    float4 Position     : SV_POSITION;
    uint RTIndex        : SV_RenderTargetArrayIndex;
};

struct SpriteVertex
{
    float4 Position     : POSITION;
    float2 Size         : SIZE;
};

struct Sprite_VSout
{
    float4 Position     : POSITION;
    float2 Size         : SIZE;
};

struct Sprite_GSout
{
    float4 Position     : SV_Position;
    float4 WorldPos     : WORLDPOSITION;
    float2 TexCoord0    : TEXCOORD0;
    uint PrimID         : SV_PrimitiveID;
};

static const float2 defaultUVs[4] =
{
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 1.0f)
};

static const float2 PoissonDisk[16] =
{
    float2(-0.613392, 0.617481),
    float2(0.170019, -0.040254),
    float2(-0.299417, 0.791925),
    float2(0.645680, 0.493210),
    float2(-0.651784, 0.717887),
    float2(0.421003, 0.027070),
    float2(-0.817194, -0.271096),
    float2(-0.705374, -0.668203),
    float2(0.977050, -0.108615),
    float2(0.063326, 0.142369),
    float2(0.203528, 0.214331),
    float2(-0.667531, 0.326090),
    float2(-0.098422, -0.295755),
    float2(-0.885922, 0.215369),
    float2(0.566637, 0.605213),
    float2(0.039766, -0.396100)
};

float RandomNumber(float4 seed)
{
    float dot_product = dot(seed, float4(12.9898, 78.233, 45.164, 94.673));
    return frac(sin(dot_product) * 43758.5453);
}

float GetLogDepth(float4 worldPosition)
{
    float4 cameraPos = mul(FB_InvView, worldPosition);
    float4 projPos = mul(FB_Projection, cameraPos);
    return projPos.z / projPos.w;
}

float RemapRange(float aValue, float aStartingLowerBound, float aStartingUpperBound, float aEndingLowerBound, float aEndingUpperBound)
{
    float startDiff = aStartingUpperBound - aStartingLowerBound;
    float endDiff = aEndingUpperBound - aEndingLowerBound;
    float remappedValue = aEndingLowerBound + (aValue - aStartingLowerBound) * endDiff / startDiff;
    return remappedValue;
}

#endif // _DEFAULT_INCLUDES_