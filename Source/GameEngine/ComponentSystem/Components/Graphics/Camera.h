#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/Math/Matrix.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Intersections/PlaneVolume.hpp"
#include "GameEngine/Intersections/AABB3D.hpp"

namespace CU = CommonUtilities;

class Transform;

class Camera : public Component
{
public:
	Camera() = default;
	Camera(float aFOV, float aNearPlane, float aFarPlane, CU::Vector2<float> aResolution);
	Camera(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar);
	void Start() override;
	void Update() override;

	void InitPerspectiveProjection(float aFOV, float aNearPlane, float aFarPlane, CU::Vector2<float> aResolution);
	void InitOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNearPlane, float aFarPlane);
	CU::Matrix4x4<float> GetProjectionMatrix() const { return myProjectionMatrix; }
	float GetNearPlane() const { return myNearPlane; }
	float GetFarPlane() const { return myFarPlane; }
	float GetHorizontalFOV() const { return myHFOV; }
	CU::Vector2f GetViewportDimensions() const { return myViewportDimensions; }

	const std::array<CU::Vector3f, 8>& GetFrustumCorners() const { return myFrustumCorners; }
	// Get Frustum Plane Volume in object space (Leave arguments empty to get in world space)
	CU::PlaneVolume<float> GetFrustumPlaneVolume(CU::Matrix4x4f aObjectSpace = CU::Matrix4x4f());
	bool GetViewcullingIntersection(std::shared_ptr<Transform> aObjectTransform, CU::AABB3D<float> aObjectAABB);

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;

protected:
	CU::Matrix4x4<float> myProjectionMatrix;
	std::array<CU::Vector3f, 8> myFrustumCorners;
	float myNearPlane = 0;
	float myFarPlane = 0;
	float myHFOV = 0;
	CU::Vector2f myViewportDimensions;
};

