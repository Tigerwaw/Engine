#ifndef _MATERIAL_BUFFER_
#define _MATERIAL_BUFFER_

cbuffer MaterialBuffer : register(b3)
{
    float4 MB_AlbedoTint;
    float MB_EmissiveStrength;
    float3 MB_Padding;
};

#endif // _MATERIAL_BUFFER_