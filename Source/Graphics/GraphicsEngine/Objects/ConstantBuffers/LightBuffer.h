#pragma once
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"
#include "GameEngine/EngineDefines.h"

namespace CU = CommonUtilities;

struct LightBuffer
{
	struct AmbientLightData
	{
		CU::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
	} AmbientLight;					// 16 bytes

	struct DirLightData
	{
		CU::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		CU::Vector3f Direction;		// 12 bytes
		int CastShadows = false;	// 4 bytes

		CU::Matrix4x4f View;		// 64 bytes
		CU::Matrix4x4f Projection;	// 64 bytes
	} DirLight;						// 160 bytes

	struct PointLightData
	{
		CU::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		CU::Vector3f Position;		// 12 bytes
		int CastShadows = false;	// 4 bytes

		CU::Matrix4x4f Projection;	// 64 bytes
	} PointLights[MAX_POINTLIGHTS];	// 96 * 4 = 384

	struct SpotLightData
	{
		CU::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		CU::Vector3f Position;		// 12 bytes
		int CastShadows = false;	// 4 bytes
		CU::Vector3f Direction;		// 12 bytes
		float ConeAngle = 0.707f;	// 4 bytes

		CU::Matrix4x4f View;		// 64 bytes
		CU::Matrix4x4f Projection;	// 64 bytes
	} SpotLights[MAX_SPOTLIGHTS];	// 176 * 4 = 704 bytes

	int NumPointLights = 0;			// 4
	int NumSpotLights = 0;			// 4

	// Total Size: 1016 bytes, missing 8
	CU::Vector2f Padding;
	// Total Size: 1024 bytes (16 * 64)
};