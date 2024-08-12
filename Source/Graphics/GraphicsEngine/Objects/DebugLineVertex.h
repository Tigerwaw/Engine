#pragma once
#include "GameEngine\Math\Vector.hpp"
namespace CU = CommonUtilities;
#include "Graphics/GraphicsEngine/RHI/RHIStructs.h"

struct DebugLineVertex
{
    CU::Vector4<float> FromPosition = { 0, 0, 0, 1 };
    CU::Vector4<float> ToPosition = { 0, 0, 0, 1 };
    CU::Vector4<float> Color = { 0, 0, 0, 1 };

	DebugLineVertex(CU::Vector3<float> fromPosition, CU::Vector3<float> toPosition, CU::Vector4<float> color)
	{
		FromPosition = CU::ToVector4(fromPosition, 1.0f);
		ToPosition = CU::ToVector4(toPosition, 1.0f);
		Color = color;
	}

    static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

