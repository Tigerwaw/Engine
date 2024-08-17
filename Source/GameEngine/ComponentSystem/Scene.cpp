#include "Enginepch.h"

#include "Scene.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Sprite.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/SpotLight.h"

#include "GameEngine/Engine.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "AssetManager/AssetManager.h"

#include "GameEngine/Math/Vector.hpp"

Scene::Scene()
{
}

Scene::~Scene()
{
	myAmbientLight = nullptr;
	myDirectionalLight = nullptr;
	myCamera = nullptr;
	myPointLights.clear();
	mySpotLights.clear();
	myGameObjects.clear();
}

void Scene::Update()
{
	myActiveGameObjectAmount = 0;

	for (auto& gameObject : myGameObjects)
	{
		if (gameObject->GetActive())
		{
			gameObject->Update();
			myActiveGameObjectAmount++;
		}
	}
}

void Scene::Render()
{
	Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(mySceneBoundingBox);
	myDirectionalLight->GetComponent<DirectionalLight>()->RecalculateShadowFrustum(myCamera, mySceneBoundingBox);
	QueueDirectionalLightShadows();
	QueuePointLightShadows();
	QueueSpotLightShadows();
	QueueUpdateLightBuffer();

	// Final Render
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PBR")->pso);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetDefaultRenderTarget>();
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(myCamera->GetComponent<Camera>());
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(126, myAmbientLight->GetComponent<AmbientLight>()->GetCubemap());
	QueueShadowmapTextureResources();


	if (GraphicsEngine::Get().GetCurrentDebugMode() != DebugMode::None)
	{
		std::shared_ptr<PipelineStateObject> pso = AssetManager::Get().GetAsset<PSOAsset>(GraphicsEngine::Get().DebugModeNames[static_cast<int>(GraphicsEngine::Get().GetCurrentDebugMode())])->pso;
		QueueGameObjects(myCamera->GetComponent<Camera>(), false, pso);
	}
	else
	{
		QueueGameObjects(myCamera->GetComponent<Camera>());
	}

	if (GraphicsEngine::Get().DrawGizmos)
	{
		QueueDebugGizmos(myCamera->GetComponent<Camera>());
	}
	
	QueueClearTextureResources();
}

std::shared_ptr<GameObject> Scene::FindGameObjectByName(std::string aName)
{
	for (auto& gameObject : myGameObjects)
	{
		if (gameObject->GetName() == aName)
		{
			return gameObject;
		}
	}

	LOG(LogScene, Warning, "Could not find game object with name {} in the scene!", aName);
	return std::shared_ptr<GameObject>();
}

void Scene::Instantiate(std::shared_ptr<GameObject> aGameObject)
{
	if (!aGameObject)
	{
		LOG(LogScene, Warning, "Tried to instantiate a non-existing gameobject!");
		return;
	}

	myGameObjects.emplace_back(aGameObject);

	if (aGameObject->GetComponent<Transform>())
	{
		CU::Vector3f bbMin = mySceneBoundingBox.GetMin();
		CU::Vector3f bbMax = mySceneBoundingBox.GetMax();
		std::shared_ptr<Transform> objectTransform = aGameObject->GetComponent<Transform>();

		if (aGameObject->GetComponent<Model>())
		{
			auto& corners = aGameObject->GetComponent<Model>()->GetBoundingBox().GetCorners();

			for (CU::Vector3f corner : corners)
			{
				corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

				bbMin.x = std::fminf(corner.x, bbMin.x);
				bbMax.x = std::fmaxf(corner.x, bbMax.x);
				bbMin.y = std::fminf(corner.y, bbMin.y);
				bbMax.y = std::fmaxf(corner.y, bbMax.y);
				bbMin.z = std::fminf(corner.z, bbMin.z);
				bbMax.z = std::fmaxf(corner.z, bbMax.z);
			}
		}
		else if (aGameObject->GetComponent<AnimatedModel>())
		{
			auto& corners = aGameObject->GetComponent<AnimatedModel>()->GetBoundingBox().GetCorners();
			for (CU::Vector3f corner : corners)
			{
				corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

				bbMin.x = std::fminf(corner.x, bbMin.x);
				bbMax.x = std::fmaxf(corner.x, bbMax.x);
				bbMin.y = std::fminf(corner.y, bbMin.y);
				bbMax.y = std::fmaxf(corner.y, bbMax.y);
				bbMin.z = std::fminf(corner.z, bbMin.z);
				bbMax.z = std::fmaxf(corner.z, bbMax.z);
			}
		}
		else
		{
			CU::Vector3f point = objectTransform->GetTranslation(true);

			bbMin.x = std::fminf(point.x, bbMin.x);
			bbMax.x = std::fmaxf(point.x, bbMax.x);
			bbMin.y = std::fminf(point.y, bbMin.y);
			bbMax.y = std::fmaxf(point.y, bbMax.y);
			bbMin.z = std::fminf(point.z, bbMin.z);
			bbMax.z = std::fmaxf(point.z, bbMax.z);
		}

		mySceneBoundingBox.InitWithMinAndMax(bbMin, bbMax);
	}

	// Temp
	if (aGameObject->GetComponent<AmbientLight>())
	{
		myAmbientLight = aGameObject;
	}
	else if (aGameObject->GetComponent<DirectionalLight>())
	{
		myDirectionalLight = aGameObject;
#ifdef _DEBUG
		myDirectionalLight->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/DirectionalLightGizmo.fbx")->mesh);
#endif
	}
	else if (aGameObject->GetComponent<PointLight>())
	{
#ifdef _DEBUG
		aGameObject->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/PointLightGizmo.fbx")->mesh);
#endif
		myPointLights.emplace_back(aGameObject);
	}
	else if (aGameObject->GetComponent<SpotLight>())
	{
#ifdef _DEBUG
		aGameObject->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/SpotLightGizmo.fbx")->mesh);
#endif
		mySpotLights.emplace_back(aGameObject);
	}
	else if (aGameObject->GetComponent<Camera>())
	{
		if (aGameObject->GetName() == "MainCamera")
		{
			myCamera = aGameObject;
		}
	}
}

void Scene::Destroy(std::shared_ptr<GameObject> aGameObject)
{
	if (!aGameObject)
	{
		LOG(LogScene, Warning, "Tried to destroy a non-existing gameobject!");
		return;
	}

	for (size_t i = 0; i < myGameObjects.size(); i++)
	{
		if (myGameObjects[i].get() == aGameObject.get())
		{
			myGameObjects.erase(myGameObjects.begin() + i);
			return;
		}
	}

	LOG(LogScene, Warning, "Could not find GameObject {} in scene!", aGameObject->GetName());
}

void Scene::QueueClearTextureResources()
{
	for (int i = 100; i < 110; i++)
	{
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ClearTextureResource>(i);
	}
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ClearTextureResource>(126);
}

void Scene::QueueShadowmapTextureResources()
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(100, myDirectionalLight->GetComponent<DirectionalLight>()->GetShadowMap());

	for (int i = 0; i < myPointLights.size(); i++)
	{
		if (i >= MAX_POINTLIGHTS) break;

		std::shared_ptr<PointLight> pLight = myPointLights[i]->GetComponent<PointLight>();
		if (!pLight->GetActive()) continue;
		if (!pLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = pLight->GetShadowMap();
		if (!shadowMap) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(101 + i, shadowMap);
	}

	for (int i = 0; i < mySpotLights.size(); i++)
	{
		if (i >= MAX_SPOTLIGHTS) break;

		std::shared_ptr<SpotLight> sLight = mySpotLights[i]->GetComponent<SpotLight>();
		if (!sLight->GetActive()) continue;
		if (!sLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = sLight->GetShadowMap();
		if (!shadowMap) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(105 + i, shadowMap);
	}
}

void Scene::QueueUpdateLightBuffer()
{
	std::shared_ptr<AmbientLight> ambientLight = myAmbientLight->GetComponent<AmbientLight>();
	std::shared_ptr<DirectionalLight> dirLight = myDirectionalLight->GetComponent<DirectionalLight>();
	std::vector<std::shared_ptr<PointLight>> pointLights;
	for (auto& plight : myPointLights)
	{
		if (plight->GetActive())
		{
			pointLights.emplace_back(plight->GetComponent<PointLight>());
		}
	}
	std::vector<std::shared_ptr<SpotLight>> spotLights;
	for (auto& slight : mySpotLights)
	{
		if (slight->GetActive())
		{
			spotLights.emplace_back(slight->GetComponent<SpotLight>());
		}
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateLightBuffer>(ambientLight, dirLight, pointLights, spotLights);
}

void Scene::QueueSpotLightShadows()
{
	for (int i = 0; i < mySpotLights.size(); i++)
	{
		std::shared_ptr<SpotLight> spotLight = mySpotLights[i]->GetComponent<SpotLight>();
		if (!spotLight->GetActive()) continue;
		if (!spotLight->CastsShadows()) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(mySpotLights[i]->GetComponent<Camera>());
		QueueGameObjects(mySpotLights[i]->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
	}
}

void Scene::QueuePointLightShadows()
{
	for (int i = 0; i < myPointLights.size(); i++)
	{
		std::shared_ptr<PointLight> pointLight = myPointLights[i]->GetComponent<PointLight>();
		if (!pointLight->GetActive()) continue;
		if (!pointLight->CastsShadows()) continue;
		
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, pointLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("ShadowCube")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(myPointLights[i]->GetComponent<Camera>());
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateShadowBuffer>(pointLight);
		QueueGameObjects(myPointLights[i]->GetComponent<Camera>(), true, AssetManager::Get().GetAsset<PSOAsset>("ShadowCube")->pso);
	}
}

void Scene::QueueDirectionalLightShadows()
{
	std::shared_ptr<DirectionalLight> dLight = myDirectionalLight->GetComponent<DirectionalLight>();
	if (!dLight->GetActive()) return;
	if (!dLight->CastsShadows()) return;
	
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetShadowMap(), false, true);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(myDirectionalLight->GetComponent<Camera>());
	QueueGameObjects(myDirectionalLight->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
}

void Scene::QueueDebugGizmos(std::shared_ptr<Camera> aRenderCamera)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("Gizmo")->pso);
	CU::PlaneVolume<float> frustumVolume = aRenderCamera->GetFrustumPlaneVolume();

	for (auto& gameObject : myGameObjects)
	{
		std::shared_ptr<DebugModel> model = gameObject->GetComponent<DebugModel>();
		if (model && model->GetActive())
		{
			if (aRenderCamera->GetViewcullingIntersection(gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderDebugMesh>(model);
			}
		}
	}
}

void Scene::QueueGameObjects(std::shared_ptr<Camera> aRenderCamera, bool disableViewCulling, std::shared_ptr<PipelineStateObject> aPSOoverride)
{
	for (auto& gameObject : myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (disableViewCulling || !GraphicsEngine::Get().UseViewCulling || !model->GetShouldViewcull() || aRenderCamera->GetViewcullingIntersection(gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model, aPSOoverride);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (disableViewCulling || !GraphicsEngine::Get().UseViewCulling || !animModel->GetShouldViewcull() || aRenderCamera->GetViewcullingIntersection(gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel, aPSOoverride);
			}
		}

		if (GraphicsEngine::Get().DrawCameraFrustums)
		{
			std::shared_ptr<Camera> cam = gameObject->GetComponent<Camera>();
			if (cam && cam->GetActive() && gameObject != myCamera)
			{
				Engine::GetInstance().GetDebugDrawer().DrawCameraFrustum(cam);
			}
		}
	}
}