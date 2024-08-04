#ifndef _LIGHT_BUFFER_
#define _LIGHT_BUFFER_

cbuffer LightBuffer : register(b4)
{
    struct AmbientLightData
    {
        float3 Color;           // 12 bytes
        float Intensity;        // 4 bytes
    } LB_AmbientLight;          // 16 bytes

    struct DirLightData
    {
        float3 Color;           // 12 bytes
        float Intensity;        // 4 bytes
        float3 Direction;       // 16 bytes
        int CastShadows;        // 4 bytes
        float MinBias;          // 4 bytes
        float MaxBias;          // 4 bytes
        int ShadowSamples;      // 4 bytes
        float Padding;         // 4 bytes

        float4x4 View;          // 64 bytes
        float4x4 Projection;    // 64 bytes
    } LB_DirLight;              // 176 bytes
	
    struct PointLightData
    {
        float3 Color;           // 12 bytes
        float Intensity;        // 4 bytes
        float3 Position;        // 12 bytes
        int CastShadows;        // 4 bytes
        float MinBias;          // 4 bytes
        float MaxBias;          // 4 bytes
        int ShadowSamples;      // 4 bytes
        float Padding;         // 4 bytes
        
        float4x4 Projection;    // 64 bytes
    } LB_PointLights[4];        // 112 * 4 = 448

    struct SpotLightData
    {
        float3 Color;           // 12 bytes
        float Intensity;        // 4 bytes
        float3 Position;        // 12 bytes
        int CastShadows;        // 4 bytes
        float3 Direction;       // 12 bytes
        float ConeAngle;        // 4 bytes
        float MinBias;          // 4 bytes
        float MaxBias;          // 4 bytes
        int ShadowSamples;      // 4 bytes
        float Padding;         // 4 bytes

        float4x4 View;          // 64 bytes
        float4x4 Projection;    // 64 bytes
    } LB_SpotLights[4];         // 192 * 4 = 768 bytes

    int LB_NumPointLights;      // 4 bytes
    int LB_NumSpotLights;       // 4 bytes

	// Total Size: 1416 bytes, missing 8
    float2 LB_Padding;
    // Total Size: 1424 bytes (16 * 89)
}

#endif // _LIGHT_BUFFER_