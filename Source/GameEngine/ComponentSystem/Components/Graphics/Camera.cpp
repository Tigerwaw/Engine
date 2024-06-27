#include "Camera.h"

#define PI 3.14159265358979323846

Camera::Camera(float aFOV, float aNearPlane, float aFarPlane, CU::Vector2<float> aResolution)
{
	InitPerspectiveProjection(aFOV, aNearPlane, aFarPlane, aResolution);
}

Camera::Camera(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar)
{
	InitOrtographicProjection(aLeft, aRight, aTop, aBottom, aNear, aFar);
}

void Camera::Start()
{
}

void Camera::Update()
{
}

void Camera::InitPerspectiveProjection(float aFOV, float aNearPlane, float aFarPlane, CU::Vector2<float> aResolution)
{
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;
	float fov = aFOV * static_cast<float>((PI / 180));
	float aspectRatio = aResolution.x / aResolution.y;
	float horizontalFOV = 1 / tanf(fov / 2);
	myHFOV = horizontalFOV;
	float verticalFOV = horizontalFOV * aspectRatio;

	myProjectionMatrix = CU::Matrix4x4<float>();
	myProjectionMatrix(1, 1) = horizontalFOV;
	myProjectionMatrix(2, 2) = verticalFOV;
	myProjectionMatrix(3, 3) = aFarPlane / (aFarPlane - aNearPlane);
	myProjectionMatrix(3, 4) = 1.0f;
	myProjectionMatrix(4, 3) = -aNearPlane * myProjectionMatrix(3, 3);
	myProjectionMatrix(4, 4) = 0;
}

void Camera::InitOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNearPlane, float aFarPlane)
{
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;
	myProjectionMatrix = CU::Matrix4x4<float>();
	myProjectionMatrix(1, 1) = 2.0f / (aRight - aLeft);
	myProjectionMatrix(2, 2) = 2.0f / (aTop - aBottom);
	myProjectionMatrix(3, 3) = 1.0f / (aFarPlane - aNearPlane);

	myProjectionMatrix(4, 1) = -(aRight + aLeft) / (aRight - aLeft);
	myProjectionMatrix(4, 2) = -(aTop + aBottom) / (aTop - aBottom);
	myProjectionMatrix(4, 3) = -(aNearPlane) / (aFarPlane - aNearPlane);
}