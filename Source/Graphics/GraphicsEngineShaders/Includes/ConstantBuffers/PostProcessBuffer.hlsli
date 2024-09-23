#ifndef _POSTPROCESS_BUFFER_
#define _POSTPROCESS_BUFFER_

cbuffer PostProcessBuffer : register(b7)
{
    float4 PPB_RandomKernel[64];
    int PPB_KernelSize = 64;
    bool PPB_SSAOEnabled;
    int PPB_BloomFunction;
    int PPB_LuminanceFunction;
    float PPB_SSAONoisePower;
    float PPB_SSAORadius;
    float PPB_SSAOBias;
    float PPB_BloomStrength;
};

#endif // _POSTPROCESS_BUFFER_