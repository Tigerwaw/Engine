#pragma once
#include "GameEngine/EngineDefines.h"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"

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
		float MinBias = 0.0001f;	// 4 bytes
		float MaxBias = 0.0005f;	// 4 bytes
		float LightSize = 1.0;		// 4 bytes
		float NearPlane;			// 4 bytes
		CU::Vector2f FrustumSize;	// 8 bytes
		CU::Vector2f Padding;		// 8 bytes

		CU::Matrix4x4f View;		// 64 bytes
		CU::Matrix4x4f Projection;	// 64 bytes
	} DirLight;						// 192 bytes

	struct PointLightData
	{
		CU::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		CU::Vector3f Position;		// 12 bytes
		int CastShadows = false;	// 4 bytes
		float MinBias = 0.0001f;	// 4 bytes
		float MaxBias = 0.0003f;	// 4 bytes
		float LightSize = 1.0;		// 4 bytes
		float NearPlane;			// 4 bytes
		CU::Vector2f FrustumSize;	// 8 bytes
		CU::Vector2f Padding;		// 8 bytes

		CU::Matrix4x4f Projection;	// 64 bytes
	} PointLights[MAX_POINTLIGHTS];	// 128 * 4 = 512

	struct SpotLightData
	{
		CU::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		CU::Vector3f Position;		// 12 bytes
		int CastShadows = false;	// 4 bytes
		CU::Vector3f Direction;		// 12 bytes
		float ConeAngle = 0.707f;	// 4 bytes
		float MinBias = 0.0001f;	// 4 bytes
		float MaxBias = 0.0005f;	// 4 bytes
		float LightSize = 1.0;		// 4 bytes
		float NearPlane;			// 4 bytes
		CU::Vector2f FrustumSize;	// 8 bytes
		CU::Vector2f Padding;		// 8 bytes

		CU::Matrix4x4f View;		// 64 bytes
		CU::Matrix4x4f Projection;	// 64 bytes
	} SpotLights[MAX_SPOTLIGHTS];	// 208 * 4 = 832 bytes

	int NumPointLights = 0;			// 4 bytes
	int NumSpotLights = 0;			// 4 bytes

	// Total Size: 1560 bytes, missing 8
	CU::Vector2f Padding;
	// Total Size: 1568 bytes (16 * 98)
};