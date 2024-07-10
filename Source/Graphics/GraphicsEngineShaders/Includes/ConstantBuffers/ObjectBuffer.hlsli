#ifndef _OBJECT_BUFFER_
#define _OBJECT_BUFFER_

cbuffer ObjectBuffer : register(b1)
{
    float4x4 OB_World;
    float4x4 OB_WorldInvT;
    bool OB_HasSkinning;
    float3 OB_Padding;
}

#endif // _OBJECT_BUFFER_