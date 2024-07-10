#ifndef _SHADOW_BUFFER_
#define _SHADOW_BUFFER_

cbuffer ShadowBuffer : register(b5)
{
    float4x4 SB_CameraTransforms[6]; // 64 * 6 = 384 bytes
}

#endif // _SHADOW_BUFFER_