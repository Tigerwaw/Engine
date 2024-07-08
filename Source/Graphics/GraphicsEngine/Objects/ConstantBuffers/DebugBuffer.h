#pragma once
#include "GameEngine/EngineDefines.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

struct DebugBuffer
{
	struct LineData
	{
		CU::Vector4f From;			// 16 bytes
		CU::Vector4f To;			// 16 bytes
		CU::Vector4f Color;			// 16 bytes
	} Lines[MAX_DEBUG_LINES];		// 48 * 256 = 12 288
};