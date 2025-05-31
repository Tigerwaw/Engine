#include "GraphicsEngine.pch.h"
#include "UpdateShadowBuffer.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "Objects/ConstantBuffers/ShadowBuffer.h"
#include "ComponentSystem/Components/Lights/PointLight.h"
#include "Math/Quaternion.hpp"

UpdateShadowBuffer::UpdateShadowBuffer(std::shared_ptr<PointLight> aPointLight)
{
	myPointLight = aPointLight;
}

void UpdateShadowBuffer::Execute()
{
	if (!myPointLight->GetActive()) return;

	ShadowBuffer shadowBufferData;

	Math::Matrix4x4f cameraTransform = myPointLight->gameObject->GetComponent<Transform>()->GetMatrix();
	cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0, 90.0f, 0 }) * cameraTransform;
	shadowBufferData.CameraTransforms[0] = cameraTransform.GetFastInverse();
	
	cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0, -180.0f, 0 }) * cameraTransform;
	shadowBufferData.CameraTransforms[1] = cameraTransform.GetFastInverse();
	
	cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ -90.0f, 90.0f, 0 }) * cameraTransform;
	shadowBufferData.CameraTransforms[2] = cameraTransform.GetFastInverse();
	 
	cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 180.0f, 0, 0 }) * cameraTransform;
	shadowBufferData.CameraTransforms[3] = cameraTransform.GetFastInverse();

	cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ -90.0f, 0, 0 }) * cameraTransform;
	shadowBufferData.CameraTransforms[4] = cameraTransform.GetFastInverse();
	
	cameraTransform = Math::Matrix4x4f::CreateRollPitchYawMatrix({ 0, 180.0f, 0 }) * cameraTransform;
	shadowBufferData.CameraTransforms[5] = cameraTransform.GetFastInverse();

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ShadowBuffer, shadowBufferData);
}

void UpdateShadowBuffer::Destroy()
{
	myPointLight = nullptr;
}
