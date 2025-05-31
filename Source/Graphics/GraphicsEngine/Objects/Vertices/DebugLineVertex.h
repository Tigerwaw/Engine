#pragma once
#include "Math/Vector.hpp"

#include "RHI/RHIStructs.h"

struct DebugLineVertex
{
    Math::Vector4<float> FromPosition = { 0, 0, 0, 1 };
    Math::Vector4<float> ToPosition = { 0, 0, 0, 1 };
    Math::Vector4<float> Color = { 0, 0, 0, 1 };

	DebugLineVertex(const Math::Vector3f& fromPosition, const Math::Vector3f& toPosition, const Math::Vector4f& color)
	{
		FromPosition = Math::ToVector4(fromPosition, 1.0f);
		ToPosition = Math::ToVector4(toPosition, 1.0f);
		Color = color;
	}

    static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

