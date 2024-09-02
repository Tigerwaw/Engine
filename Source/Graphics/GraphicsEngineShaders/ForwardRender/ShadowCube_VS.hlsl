#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/AnimationBuffer.hlsli"
#include "../Includes/ConstantBuffers/ObjectBuffer.hlsli"

ShadowCube_VSout main(MeshVertex vertex)
{
    ShadowCube_VSout result;
    
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
    }
    
    float4 skinnedPos = mul(skinMatrix, vertex.Position);
    result.Position = mul(OB_World, skinnedPos);
    return result;
}