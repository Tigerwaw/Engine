#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/Math/Matrix.hpp"
#include "GameEngine/Math/Vector.hpp"

namespace CU = CommonUtilities;

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
protected:
	CU::Matrix4x4<float> myProjectionMatrix;
	float myNearPlane = 0;
	float myFarPlane = 0;
	float myHFOV = 0;
};

