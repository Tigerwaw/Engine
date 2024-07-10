#ifndef _ANIMATION_BUFFER_
#define _ANIMATION_BUFFER_

cbuffer AnimationBuffer : register(b2)
{
    float4x4 AB_JointTransforms[128];
}

#endif // _ANIMATION_BUFFER_