#ifndef _DEBUG_INCLUDES_
#define _DEBUG_INCLUDES_

struct DebugLineVertex
{
    float4 From         : FROM;
    float4 To           : TO;
    float4 Color        : COLOR;
};

struct DebugLine_VSout
{
    float4 From         : FROM;
    float4 To           : TO;
    float4 Color        : COLOR;
};

struct DebugObject_GSout
{
    float4 Position     : SV_Position;
    float4 Color        : COLOR;
};

#endif // _DEBUG_INCLUDES_