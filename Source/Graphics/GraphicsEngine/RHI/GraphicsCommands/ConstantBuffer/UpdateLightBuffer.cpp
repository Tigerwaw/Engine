#include "GraphicsEngine.pch.h"
#include "UpdateLightBuffer.h"
#include "Graphics/GraphicsEngine/Objects/ConstantBuffers/LightBuffer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
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
		lightBufferData.DirLight.Color = myDirectionalLight->GetColor();
		lightBufferData.DirLight.Intensity = myDirectionalLight->GetIntensity();
		lightBufferData.DirLight.Direction = myDirectionalLight->GetDirection();
		lightBufferData.DirLight.View = myDirectionalLight->GetParent()->Transform.GetMatrix().GetFastInverse();
		lightBufferData.DirLight.Projection = myDirectionalLight->GetParent()->GetComponent<Camera>()->GetProjectionMatrix();
		lightBufferData.DirLight.CastShadows = myDirectionalLight->CastsShadows();
		lightBufferData.DirLight.MinBias = myDirectionalLight->GetMinShadowBias();
		lightBufferData.DirLight.MaxBias = myDirectionalLight->GetMaxShadowBias();
	}

	int pIndex = 0;
	for (pIndex = 0; pIndex < myPointLights.size(); pIndex++)
	{
		std::shared_ptr<PointLight> pointLight = myPointLights[pIndex];

		if (!pointLight->GetActive()) continue;

		lightBufferData.PointLights[pIndex].Position = pointLight->GetPosition();
		lightBufferData.PointLights[pIndex].Color = pointLight->GetColor();
		lightBufferData.PointLights[pIndex].Intensity = pointLight->GetIntensity();
		lightBufferData.PointLights[pIndex].CastShadows = pointLight->CastsShadows();
		lightBufferData.PointLights[pIndex].MinBias = pointLight->GetMinShadowBias();
		lightBufferData.PointLights[pIndex].MaxBias = pointLight->GetMaxShadowBias();
		
		lightBufferData.PointLights[pIndex].Projection = pointLight->GetParent()->GetComponent<Camera>()->GetProjectionMatrix();
	}
	lightBufferData.NumPointLights = pIndex;

	int sIndex = 0;
	for (sIndex = 0; sIndex < mySpotLights.size(); sIndex++)
	{
		std::shared_ptr<SpotLight> spotLight = mySpotLights[sIndex];

		if (!spotLight->GetActive()) continue;

		lightBufferData.SpotLights[sIndex].Position = spotLight->GetPosition();
		lightBufferData.SpotLights[sIndex].Direction = spotLight->GetDirection();
		lightBufferData.SpotLights[sIndex].ConeAngle = spotLight->GetConeAngleRadians();
		lightBufferData.SpotLights[sIndex].Color = spotLight->GetColor();
		lightBufferData.SpotLights[sIndex].Intensity = spotLight->GetIntensity();
		lightBufferData.SpotLights[sIndex].CastShadows = spotLight->CastsShadows();
		lightBufferData.SpotLights[sIndex].MinBias = spotLight->GetMinShadowBias();
		lightBufferData.SpotLights[sIndex].MaxBias = spotLight->GetMaxShadowBias();

		lightBufferData.SpotLights[sIndex].View = spotLight->GetParent()->Transform.GetMatrix().GetFastInverse();
		lightBufferData.SpotLights[sIndex].Projection = spotLight->GetParent()->GetComponent<Camera>()->GetProjectionMatrix();
	}
	lightBufferData.NumSpotLights = sIndex;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::LightBuffer, lightBufferData);
}

void UpdateLightBuffer::Destroy()
{
	myAmbientLight = nullptr;
	myDirectionalLight = nullptr;
	myPointLights.clear();
	mySpotLights.clear();
}
