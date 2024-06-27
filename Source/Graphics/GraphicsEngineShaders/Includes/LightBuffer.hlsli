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

        float4x4 View;          // 64 bytes
        float4x4 Projection;    // 64 bytes
    } LB_DirLight;              // 160 bytes
	
    struct PointLightData
    {
        float3 Color;           // 12 bytes
        float Intensity;        // 4 bytes
        float3 Position;        // 12 bytes
        int CastShadows;        // 4 bytes
        
        float4x4 Projection;    // 64 bytes
    } LB_PointLights[4];        // 96 * 4 = 384

    struct SpotLightData
    {
        float3 Color;           // 12 bytes
        float Intensity;        // 4 bytes
        float3 Position;        // 12 bytes
        int CastShadows;        // 4 bytes
        float3 Direction;       // 12 bytes
        float ConeAngle;        // 4 bytes

        float4x4 View;          // 64 bytes
        float4x4 Projection;    // 64 bytes
    } LB_SpotLights[4];         // 176 * 4 = 704 bytes

    int LB_NumPointLights;      // 4
    int LB_NumSpotLights;       // 4

	// Total Size: 1016 bytes, missing 8
    float2 LB_Padding;
    // Total Size: 1024 bytes (16 * 64)
}