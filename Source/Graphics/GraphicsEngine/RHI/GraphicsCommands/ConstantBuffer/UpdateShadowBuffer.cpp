#include "GraphicsEngine.pch.h"
#include "UpdateShadowBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "Graphics/GraphicsEngine/Objects/ConstantBuffers/ShadowBuffer.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"

UpdateShadowBuffer::UpdateShadowBuffer(std::shared_ptr<PointLight> aPointLight)
{
	myPointLight = aPointLight;
}

void UpdateShadowBuffer::Execute()
{
	if (!myPointLight->GetActive()) return;

	ShadowBuffer shadowBufferData;
	CU::Transform<float> cameraTransform = myPointLight->GetParent()->Transform;
	cameraTransform.AddRotation(0, 90.0f, 0);
	shadowBufferData.CameraTransforms[0] = cameraTransform.GetMatrix().GetFastInverse();
	cameraTransform.AddRotation(0, -180.0f, 0);
	shadowBufferData.CameraTransforms[1] = cameraTransform.GetMatrix().GetFastInverse();
	cameraTransform.AddRotation(0, 90.0f, 0);
	cameraTransform.AddRotation(-90.0f, 0, 0);
	shadowBufferData.CameraTransforms[2] = cameraTransform.GetMatrix().GetFastInverse();
	cameraTransform.AddRotation(180.0f, 0, 0);
	shadowBufferData.CameraTransforms[3] = cameraTransform.GetMatrix().GetFastInverse();
	cameraTransform.AddRotation(-90.0f, 0, 0);
	shadowBufferData.CameraTransforms[4] = cameraTransform.GetMatrix().GetFastInverse();
	cameraTransform.AddRotation(0, 180.0f, 0);
	shadowBufferData.CameraTransforms[5] = cameraTransform.GetMatrix().GetFastInverse();

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ShadowBuffer, shadowBufferData);
}

void UpdateShadowBuffer::Destroy()
{
	myPointLight = nullptr;
}
