#include "GraphicsEngine.pch.h"
#include "UpdateFrameBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/FrameBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

UpdateFrameBuffer::UpdateFrameBuffer(std::shared_ptr<Camera> aCamera)
{
	myCameraInvView = aCamera->gameObject->GetComponent<Transform>()->GetWorldMatrix().GetFastInverse();
	myCameraProj = aCamera->GetProjectionMatrix();
	myViewPosition = CU::ToVector4(aCamera->gameObject->GetComponent<Transform>()->GetTranslation(true), 1.0f);
	myViewDirection = CU::ToVector4(aCamera->gameObject->GetComponent<Transform>()->GetForwardVector(true), 1.0f);
	myNearPlane = aCamera->GetNearPlane();
	myFarPlane = aCamera->GetFarPlane();
	myTotalTime = static_cast<float>(Engine::GetInstance().GetTimer().GetTotalTime());
	myDeltaTime = Engine::GetInstance().GetTimer().GetDeltaTime();
	myResolution = Engine::GetInstance().GetResolution();
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