#pragma once
#include "GameEngine\Math\Vector.hpp"
namespace CU = CommonUtilities;
#include "Graphics/GraphicsEngine/RHI/RHIStructs.h"

struct DebugLineVertex
{
    CU::Vector4<float> FromPosition = { 0, 0, 0, 1 };
    CU::Vector4<float> ToPosition = { 0, 0, 0, 1 };
    CU::Vector4<float> Color = { 0, 0, 0, 1 };

	DebugLineVertex(const CU::Vector3f& fromPosition, const CU::Vector3f& toPosition, const CU::Vector4f& color)
	{
		FromPosition = CU::ToVector4(fromPosition, 1.0f);
		ToPosition = CU::ToVector4(toPosition, 1.0f);
		Color = color;
	}

    static const std::vector<VertexElementDesc> InputLayoutDefinition;
};

