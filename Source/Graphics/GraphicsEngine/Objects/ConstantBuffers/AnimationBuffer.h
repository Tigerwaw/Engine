#pragma once
#include "Math/Matrix4x4.hpp"

struct AnimationBuffer
{
    Math::Matrix4x4f JointTransforms[128];
};