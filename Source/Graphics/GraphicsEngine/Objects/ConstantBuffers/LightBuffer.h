#pragma once
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

#define MAX_POINTLIGHTS 4
#define MAX_SPOTLIGHTS 4
#define MAX_DEBUG_LINES 2048
#define MAX_PARTICLES 20000


struct LightBuffer
{
	struct AmbientLightData
	{
		Math::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
	} AmbientLight;					// 16 bytes

	struct DirLightData
	{
		Math::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		Math::Vector3f Direction;		// 12 bytes
		int CastShadows = false;	// 4 bytes
		float MinBias = 0.0001f;	// 4 bytes
		float MaxBias = 0.0005f;	// 4 bytes
		float LightSize = 1.0f;		// 4 bytes
		float NearPlane = 1.0f;		// 4 bytes
		Math::Vector2f FrustumSize;	// 8 bytes
		Math::Vector2f Padding;		// 8 bytes

		Math::Matrix4x4f View;		// 64 bytes
		Math::Matrix4x4f Projection;	// 64 bytes
	} DirLight;						// 192 bytes

	struct PointLightData
	{
		Math::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		Math::Vector3f Position;		// 12 bytes
		int CastShadows = false;	// 4 bytes
		float MinBias = 0.0001f;	// 4 bytes
		float MaxBias = 0.0003f;	// 4 bytes
		float LightSize = 1.0f;		// 4 bytes
		float NearPlane = 1.0f;		// 4 bytes
		Math::Vector2f FrustumSize;	// 8 bytes
		Math::Vector2f Padding;		// 8 bytes

		Math::Matrix4x4f Projection;	// 64 bytes
	} PointLights[MAX_POINTLIGHTS];	// 128 * 4 = 512

	struct SpotLightData
	{
		Math::Vector3f Color;			// 12 bytes
		float Intensity = 1.0f;		// 4 bytes
		Math::Vector3f Position;		// 12 bytes
		int CastShadows = false;	// 4 bytes
		Math::Vector3f Direction;		// 12 bytes
		float ConeAngle = 0.707f;	// 4 bytes
		float MinBias = 0.0001f;	// 4 bytes
		float MaxBias = 0.0005f;	// 4 bytes
		float LightSize = 1.0f;		// 4 bytes
		float NearPlane = 1.0f;		// 4 bytes
		Math::Vector2f FrustumSize;	// 8 bytes
		Math::Vector2f Padding;		// 8 bytes

		Math::Matrix4x4f View;		// 64 bytes
		Math::Matrix4x4f Projection;	// 64 bytes
	} SpotLights[MAX_SPOTLIGHTS];	// 208 * 4 = 832 bytes

	int NumPointLights = 0;			// 4 bytes
	int NumSpotLights = 0;			// 4 bytes

	// Total Size: 1560 bytes, missing 8
	Math::Vector2f Padding;
	// Total Size: 1568 bytes (16 * 98)
};