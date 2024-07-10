#ifndef _MATERIAL_BUFFER_
#define _MATERIAL_BUFFER_

cbuffer MaterialBuffer : register(b3)
{
    float4 MB_AlbedoTint;
};

#endif // _MATERIAL_BUFFER_