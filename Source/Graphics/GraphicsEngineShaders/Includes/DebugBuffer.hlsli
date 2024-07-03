cbuffer DebugBuffer : register(b7)
{
    struct LineData
    {
        float4 From;            // 16 bytes
        float4 To;              // 16 bytes
        float4 Color;           // 16 bytes
    } DB_Lines[128];            // 48 * 128 = 6144
}