cbuffer ShadowBuffer : register(b5)
{
    float4x4 SB_CameraTransforms[6]; // 64 * 6 = 384 bytes
}