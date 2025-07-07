#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "Math/PlaneVolume.hpp"
#include "Math/AABB3D.hpp"



class Transform;

class Camera : public Component
{
public:
	Camera() = default;
	Camera(float aFOV, float aNearPlane, float aFarPlane, Math::Vector2f aResolution);
	Camera(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar);
	void Start() override;
	void Update() override;

	void InitPerspectiveProjection(float aFOV, float aNearPlane, float aFarPlane, Math::Vector2f aResolution);
	void InitOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNearPlane, float aFarPlane);
	Math::Matrix4x4f GetProjectionMatrix() const { return myProjectionMatrix; }
	float GetNearPlane() const { return myNearPlane; }
	float GetFarPlane() const { return myFarPlane; }
	float GetHorizontalFOV() const { return myHFOV; }
	Math::Vector2f GetViewportDimensions() const { return myViewportDimensions; }
	void SetAsMainCamera(bool aIsMainCamera) { myIsMainCamera = aIsMainCamera; }
	bool IsMainCamera() const { return myIsMainCamera; }

	const std::array<Math::Vector3f, 8>& GetFrustumCorners() const { return myFrustumCorners; }
	// Get Frustum Plane Volume in object space (Leave arguments empty to get in world space)
	Math::PlaneVolume<float> GetFrustumPlaneVolume(Math::Matrix4x4f aObjectSpace = Math::Matrix4x4f());
	bool GetViewcullingIntersection(std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB);

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;

protected:
	bool myIsMainCamera = false;
	Math::Matrix4x4f myProjectionMatrix;
	std::array<Math::Vector3f, 8> myFrustumCorners;
	float myNearPlane = 0;
	float myFarPlane = 0;
	float myHFOV = 0;
	Math::Vector2f myViewportDimensions;
};

