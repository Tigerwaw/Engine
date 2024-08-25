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
        float LightSize;        // 4 bytes
        float2 FrustumSize;     // 8 bytes
        float NearPlane;        // 4 bytes
        float Padding;          // 4 bytes

        float4x4 View;          // 64 bytes
        float4x4 Projection;    // 64 bytes
    } LB_DirLight;              // 192 bytes
	
    struct PointLightData
    {
        float3 Color;           // 12 bytes
        float Intensity;        // 4 bytes
        float3 Position;        // 12 bytes
        int CastShadows;        // 4 bytes
        float MinBias;          // 4 bytes
        float MaxBias;          // 4 bytes
        float LightSize;		// 4 bytes
		float NearPlane;		// 4 bytes
		float2 FrustumSize;	    // 8 bytes
        float2 Padding;         // 8 bytes
        
        float4x4 Projection;    // 64 bytes
    } LB_PointLights[4];        // 128 * 4 = 512

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
        float LightSize;		// 4 bytes
		float NearPlane;		// 4 bytes
		float2 FrustumSize;	    // 8 bytes
		float2 Padding;         // 8 bytes

        float4x4 View;          // 64 bytes
        float4x4 Projection;    // 64 bytes
    } LB_SpotLights[4];         // 208 * 4 = 832 bytes

    int LB_NumPointLights;      // 4 bytes
    int LB_NumSpotLights;       // 4 bytes

	// Total Size: 1560 bytes, missing 8
    float2 LB_Padding;
    // Total Size: 1568 bytes (16 * 98)
}

#endif // _LIGHT_BUFFER_