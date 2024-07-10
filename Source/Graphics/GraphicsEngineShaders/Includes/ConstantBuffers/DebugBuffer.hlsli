#ifndef _DEBUG_BUFFER_
#define _DEBUG_BUFFER_

cbuffer DebugBuffer : register(b7)
{
    struct LineData
    {
        float4 From;            // 16 bytes
        float4 To;              // 16 bytes
        float4 Color;           // 16 bytes
    } DB_Lines[256];            // 48 * 256 = 12 288
}

#endif // _DEBUG_BUFFER_