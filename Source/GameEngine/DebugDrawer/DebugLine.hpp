#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

struct DebugLine
{
	CU::Vector3f From;
	CU::Vector3f To;
	CU::Vector4f Color;
};