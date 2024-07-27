#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/ConstantBuffers/AnimationBuffer.hlsli"
#include "Includes/ConstantBuffers/ObjectBuffer.hlsli"
#include "Includes/ConstantBuffers/FrameBuffer.hlsli"

MeshVStoPS main(MeshVertex vertex)
{
    MeshVStoPS result;
    
    result.Normal = mul((float3x3) OB_WorldInvT, normalize(vertex.Normal));
    result.Tangent = mul((float3x3) OB_WorldInvT, normalize(vertex.Tangent));
    result.Binormal = cross(result.Tangent, result.Normal);
    
    float4x4 skinMatrix =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    
    if (OB_HasSkinning)
    {
        uint boneID = 0;
        float weight = 0;
        
        boneID = vertex.BoneIDs.x;
        weight = vertex.Skinweights.x;
        skinMatrix = mul(AB_JointTransforms[boneID], weight);
        
        boneID = vertex.BoneIDs.y;
        weight = vertex.Skinweights.y;
        skinMatrix += mul(AB_JointTransforms[boneID], weight);
        
        boneID = vertex.BoneIDs.z;
        weight = vertex.Skinweights.z;
        skinMatrix += mul(AB_JointTransforms[boneID], weight);
        
        boneID = vertex.BoneIDs.w;
        weight = vertex.Skinweights.w;
        skinMatrix += mul(AB_JointTransforms[boneID], weight);
        
        float3x3 tSkinMatrix = transpose((float3x3) skinMatrix);
        result.Normal = mul(result.Normal, tSkinMatrix);
        result.Tangent = mul(result.Tangent, tSkinMatrix);
        result.Binormal = mul(result.Binormal, tSkinMatrix);
    }
    
    float4 skinnedPos = mul(skinMatrix, vertex.Position);
    result.WorldPos = mul(OB_World, skinnedPos);
    result.ViewPos = mul(FB_InvView, result.WorldPos);
    result.Position = mul(FB_Projection, result.ViewPos);
    
    result.VertexColor0 = vertex.VertexColor0;
    result.VertexColor1 = vertex.VertexColor1;
    result.VertexColor2 = vertex.VertexColor2;
    result.VertexColor3 = vertex.VertexColor3;
    result.TexCoord0 = vertex.TexCoord0;
    result.TexCoord1 = vertex.TexCoord1;
    result.TexCoord2 = vertex.TexCoord2;
    result.TexCoord3 = vertex.TexCoord3;
    return result;
}