#include "GraphicsEngine.pch.h"
#include "UpdateLightBuffer.h"
#include "Graphics/GraphicsEngine/Objects/ConstantBuffers/LightBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/SpotLight.h"

UpdateLightBuffer::UpdateLightBuffer(std::shared_ptr<AmbientLight> aAmbientLight, std::shared_ptr<DirectionalLight> aDirLight, std::vector<std::shared_ptr<PointLight>> aPointLights, std::vector<std::shared_ptr<SpotLight>> aSpotLights)
{
	myAmbientLight = aAmbientLight;
	myDirectionalLight = aDirLight;
	myPointLights = aPointLights;
	mySpotLights = aSpotLights;
}

void UpdateLightBuffer::Execute()
{
	LightBuffer lightBufferData;
	
	if (myAmbientLight->GetActive())
	{
		lightBufferData.AmbientLight.Color = myAmbientLight->GetColor();
		lightBufferData.AmbientLight.Intensity = myAmbientLight->GetIntensity();
	}

	if (myDirectionalLight->GetActive())
	{
		std::shared_ptr<Camera> cam = myDirectionalLight->gameObject->GetComponent<Camera>();

		lightBufferData.DirLight.Color = myDirectionalLight->GetColor();
		lightBufferData.DirLight.Intensity = myDirectionalLight->GetIntensity();
		lightBufferData.DirLight.Direction = myDirectionalLight->GetDirection();
		lightBufferData.DirLight.View = myDirectionalLight->gameObject->GetComponent<Transform>()->GetMatrix().GetFastInverse();
		lightBufferData.DirLight.Projection = cam->GetProjectionMatrix();
		lightBufferData.DirLight.CastShadows = myDirectionalLight->CastsShadows();
		lightBufferData.DirLight.MinBias = myDirectionalLight->GetMinShadowBias();
		lightBufferData.DirLight.MaxBias = myDirectionalLight->GetMaxShadowBias();
		lightBufferData.DirLight.LightSize = myDirectionalLight->GetLightSize();
		lightBufferData.DirLight.FrustumSize = cam->GetViewportDimensions();
		lightBufferData.DirLight.NearPlane = cam->GetNearPlane();
	}

	int pIndex = 0;
	for (pIndex = 0; pIndex < myPointLights.size(); pIndex++)
	{
		std::shared_ptr<PointLight> pointLight = myPointLights[pIndex];

		if (!pointLight->GetActive()) continue;

		std::shared_ptr<Camera> cam = pointLight->gameObject->GetComponent<Camera>();

		lightBufferData.PointLights[pIndex].Position = pointLight->GetPosition();
		lightBufferData.PointLights[pIndex].Color = pointLight->GetColor();
		lightBufferData.PointLights[pIndex].Intensity = pointLight->GetIntensity();
		lightBufferData.PointLights[pIndex].CastShadows = pointLight->CastsShadows();
		lightBufferData.PointLights[pIndex].MinBias = pointLight->GetMinShadowBias();
		lightBufferData.PointLights[pIndex].MaxBias = pointLight->GetMaxShadowBias();
		lightBufferData.PointLights[pIndex].LightSize = pointLight->GetLightSize();
		lightBufferData.PointLights[pIndex].FrustumSize = cam->GetViewportDimensions();
		lightBufferData.PointLights[pIndex].NearPlane = cam->GetNearPlane();
		
		lightBufferData.PointLights[pIndex].Projection = cam->GetProjectionMatrix();
	}
	lightBufferData.NumPointLights = pIndex;

	int sIndex = 0;
	for (sIndex = 0; sIndex < mySpotLights.size(); sIndex++)
	{
		std::shared_ptr<SpotLight> spotLight = mySpotLights[sIndex];

		if (!spotLight->GetActive()) continue;
		std::shared_ptr<Camera> cam = spotLight->gameObject->GetComponent<Camera>();

		lightBufferData.SpotLights[sIndex].Position = spotLight->GetPosition();
		lightBufferData.SpotLights[sIndex].Direction = spotLight->GetDirection();
		lightBufferData.SpotLights[sIndex].ConeAngle = spotLight->GetConeAngleRadians();
		lightBufferData.SpotLights[sIndex].Color = spotLight->GetColor();
		lightBufferData.SpotLights[sIndex].Intensity = spotLight->GetIntensity();
		lightBufferData.SpotLights[sIndex].CastShadows = spotLight->CastsShadows();
		lightBufferData.SpotLights[sIndex].MinBias = spotLight->GetMinShadowBias();
		lightBufferData.SpotLights[sIndex].MaxBias = spotLight->GetMaxShadowBias();
		lightBufferData.SpotLights[sIndex].LightSize = spotLight->GetLightSize();
		lightBufferData.SpotLights[sIndex].FrustumSize = cam->GetViewportDimensions();
		lightBufferData.SpotLights[sIndex].NearPlane = cam->GetNearPlane();
		
		lightBufferData.SpotLights[sIndex].View = spotLight->gameObject->GetComponent<Transform>()->GetMatrix().GetFastInverse();
		lightBufferData.SpotLights[sIndex].Projection = cam->GetProjectionMatrix();
	}
	lightBufferData.NumSpotLights = sIndex;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::LightBuffer, lightBufferData);
}

void UpdateLightBuffer::Destroy()
{
	myAmbientLight = nullptr;
	myDirectionalLight = nullptr;
	myPointLights.~vector();
	mySpotLights.~vector();
}
