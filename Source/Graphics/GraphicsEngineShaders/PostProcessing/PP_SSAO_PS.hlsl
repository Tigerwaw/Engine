#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/DeferredShaderIncludes.hlsli"
#include "../Includes/ConstantBuffers/PostProcessBuffer.hlsli"

float4 main(Quad_VS_to_PS input) : SV_TARGET
{
    const float4 pos = GetViewSpacePosition(input.UV);
    const float3 nrm = GetViewSpaceNormal(input.UV).rgb;
    const float3 r = GetRandomNoise(input.UV, GetNoiseScale()) * PPB_SSAONoisePower;
    const float3 T = normalize(r - nrm * dot(r, nrm));
    const float3 B = cross(nrm, T);
    const float3x3 TBN = float3x3(T, B, nrm);
    
    float occlusion = 0;
    
    const float radius = PPB_SSAORadius;
    const float bias = PPB_SSAOBias;
    for (int i = 0; i < PPB_KernelSize; ++i)
    {
        float3 kernelPos = mul(TBN, PPB_RandomKernel[i].xyz);
        kernelPos = pos.xyz + kernelPos * radius;
        float4 offset = float4(kernelPos, 1);
        offset = mul(FB_Projection, offset);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;
        offset.y = 1 - offset.y;
        
        const float stepDepth = GetViewSpacePosition(offset.xy).z;
        const float range = smoothstep(0.0, 1.0, radius / abs(pos.z - stepDepth));
        occlusion += (stepDepth >= kernelPos.z + bias ? 1.0 : 0.0) * range;
    }
    
    occlusion = 1.0f - (occlusion / PPB_KernelSize);
    return occlusion.rrrr;
}