cbuffer ObjectBuffer : register(b1)
{
    float4x4 OB_World;
    float4x4 OB_WorldInvT;
    bool OB_HasSkinning;
    float3 OB_Padding;
}