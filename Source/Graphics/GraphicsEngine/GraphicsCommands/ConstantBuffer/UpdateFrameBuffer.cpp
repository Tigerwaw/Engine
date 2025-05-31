#include "GraphicsEngine.pch.h"
#include "UpdateFrameBuffer.h"
#include "Objects/ConstantBuffers/FrameBuffer.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/Camera.h"
#include "Engine.h"
#include "Time/Timer.h"

UpdateFrameBuffer::UpdateFrameBuffer(std::shared_ptr<Camera> aCamera)
{
	myCameraInvView = aCamera->gameObject->GetComponent<Transform>()->GetWorldMatrix().GetFastInverse();
	myCameraProj = aCamera->GetProjectionMatrix();
	myViewPosition = Math::ToVector4(aCamera->gameObject->GetComponent<Transform>()->GetTranslation(true), 1.0f);
	myViewDirection = Math::ToVector4(aCamera->gameObject->GetComponent<Transform>()->GetForwardVector(true), 1.0f);
	myNearPlane = aCamera->GetNearPlane();
	myFarPlane = aCamera->GetFarPlane();
	myTotalTime = static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart());
	myDeltaTime = Engine::Get().GetTimer().GetDeltaTime();
	myResolution = Engine::Get().GetResolution();
}

void UpdateFrameBuffer::Execute()
{
	FrameBuffer frameBufferData;
	frameBufferData.InvView = myCameraInvView;
	frameBufferData.Projection = myCameraProj;
	frameBufferData.ViewPosition = myViewPosition;
	frameBufferData.ViewDirection = myViewDirection;
	frameBufferData.NearPlane = myNearPlane;
	frameBufferData.FarPlane = myFarPlane;
	frameBufferData.Time = { myTotalTime, myDeltaTime };
	frameBufferData.Resolution = myResolution;
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, frameBufferData);
}

void UpdateFrameBuffer::Destroy()
{
}