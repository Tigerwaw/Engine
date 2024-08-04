#include "Camera.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/Intersections/Intersection3D.hpp"

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

	//CU::Vector3f topLeft = CU::Vector3f(-cos(horizontalFOV), sin(verticalFOV), 1.0f).GetNormalized();
	//CU::Vector3f topRight = CU::Vector3f(cos(horizontalFOV), sin(verticalFOV), 1.0f).GetNormalized();
	//CU::Vector3f bottomRight = CU::Vector3f(cos(horizontalFOV), -sin(verticalFOV), 1.0f).GetNormalized();
	//CU::Vector3f bottomLeft = CU::Vector3f(-cos(horizontalFOV), -sin(verticalFOV), 1.0f).GetNormalized();
	
	CU::Vector3f topLeft = CU::Vector3f(-1.0f, 1.0f, 1.0f).GetNormalized();
	CU::Vector3f topRight = CU::Vector3f(1.0f, 1.0f, 1.0f).GetNormalized();
	CU::Vector3f bottomRight = CU::Vector3f(1.0f, -1.0f, 1.0f).GetNormalized();
	CU::Vector3f bottomLeft = CU::Vector3f(-1.0f, -1.0f, 1.0f).GetNormalized();

	myFrustumCorners[0] = bottomLeft * aNearPlane;
	myFrustumCorners[1] = topLeft * aNearPlane;
	myFrustumCorners[2] = topRight * aNearPlane;
	myFrustumCorners[3] = bottomRight * aNearPlane;
	myFrustumCorners[4] = bottomLeft * aFarPlane;
	myFrustumCorners[5] = topLeft * aFarPlane;
	myFrustumCorners[6] = topRight * aFarPlane;
	myFrustumCorners[7] = bottomRight * aFarPlane;
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

	myFrustumCorners[0] = CU::Vector3f(aLeft, aBottom, aNearPlane);
	myFrustumCorners[1] = CU::Vector3f(aLeft, aTop, aNearPlane);
	myFrustumCorners[2] = CU::Vector3f(aRight, aTop, aNearPlane);
	myFrustumCorners[3] = CU::Vector3f(aRight, aBottom, aNearPlane);
	myFrustumCorners[4] = CU::Vector3f(aLeft, aBottom, aFarPlane);
	myFrustumCorners[5] = CU::Vector3f(aLeft, aTop, aFarPlane);
	myFrustumCorners[6] = CU::Vector3f(aRight, aTop, aFarPlane);
	myFrustumCorners[7] = CU::Vector3f(aRight, aBottom, aFarPlane);
}

// Does not work with scale yet.
CU::PlaneVolume<float> Camera::GetFrustumPlaneVolume(CU::Matrix4x4f aToObjectSpace)
{
	std::vector<CU::Vector3f> corners;
	
	CU::Matrix4x4f matrix = gameObject->GetComponent<Transform>()->GetWorldMatrix();
	if (aToObjectSpace != CU::Matrix4x4f())
	{
		matrix = matrix * aToObjectSpace;
	}

	for (auto& corner : myFrustumCorners)
	{
		corners.emplace_back(CU::ToVector3(CU::ToVector4(corner, 1.0f) * matrix));
	}

	CU::PlaneVolume<float> volume;
	volume.AddPlane(CU::Plane<float>(corners[0], corners[1], corners[2]));
	volume.AddPlane(CU::Plane<float>(corners[6], corners[5], corners[4]));
	
	volume.AddPlane(CU::Plane<float>(corners[4], corners[5], corners[1]));
	volume.AddPlane(CU::Plane<float>(corners[2], corners[6], corners[7]));
	
	volume.AddPlane(CU::Plane<float>(corners[7], corners[4], corners[0]));
	volume.AddPlane(CU::Plane<float>(corners[1], corners[5], corners[6]));

	return volume;
}

bool Camera::GetViewcullingIntersection(std::shared_ptr<Transform> aObjectTransform, CU::AABB3D<float> aObjectAABB)
{
	std::shared_ptr<Transform> goTransform = gameObject->GetComponent<Transform>();
	CU::Matrix4x4f objectMatrix = aObjectTransform->GetWorldMatrix();

	if (goTransform->IsScaled())
	{
		objectMatrix = objectMatrix.GetInverse();
	}
	else
	{
		objectMatrix = objectMatrix.GetFastInverse();
	}

	return CU::IntersectionBetweenPlaneVolumeAABB(GetFrustumPlaneVolume(objectMatrix), aObjectAABB);
}
