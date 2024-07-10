#ifndef _DEBUG_INCLUDES_
#define _DEBUG_INCLUDES_

struct DebugObject_VSout
{
    uint vertexID       : VERTEXID;
};

struct DebugObject_GSout
{
    float4 Position     : SV_Position;
    float4 Color        : COLOR;
};

#endif // _DEBUG_INCLUDES_