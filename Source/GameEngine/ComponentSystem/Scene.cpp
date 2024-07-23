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
#include "GameEngine/Intersections/Intersection3D.hpp"
#include "GameEngine/DebugDrawer/DebugDrawer.h"

#include "Logger/Logger.h"
#include "GameEngine/Math/Vector.hpp"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogScene, Scene, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogScene, Scene, Warning);
#endif

#define SCENELOG(Verbosity, Message, ...) LOG(LogScene, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogScene);

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
	QueueDirectionalLightShadows();
	QueuePointLightShadows();
	QueueSpotLightShadows();
	QueueUpdateLightBuffer();

	// Final Render
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(PipelineStateType::Default);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetDefaultRenderTarget>();
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(myCamera->GetComponent<Camera>());
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(126, myAmbientLight->GetComponent<AmbientLight>()->GetEnvironmentTexture());
	QueueShadowmapTextureResources();

	QueueGameObjects(myCamera->GetComponent<Camera>());

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

	SCENELOG(Warning, "Could not find game object with name {} in the scene!", aName);
	return std::shared_ptr<GameObject>();
}

void Scene::Instantiate(std::shared_ptr<GameObject> aGameObject)
{
	if (!aGameObject)
	{
		SCENELOG(Warning, "Tried to instantiate a non-existing gameobject!");
		return;
	}

	myGameObjects.emplace_back(aGameObject);

	// Temp
	if (aGameObject->GetComponent<AmbientLight>())
	{
		myAmbientLight = aGameObject;
	}
	else if (aGameObject->GetComponent<DirectionalLight>())
	{
		myDirectionalLight = aGameObject;
	}
	else if (aGameObject->GetComponent<PointLight>())
	{
		myPointLights.emplace_back(aGameObject);
	}
	else if (aGameObject->GetComponent<SpotLight>())
	{
		mySpotLights.emplace_back(aGameObject);
	}
	else if (aGameObject->GetComponent<Camera>())
	{
		myCamera = aGameObject;
	}
}

void Scene::Destroy(std::shared_ptr<GameObject> aGameObject)
{
	if (!aGameObject)
	{
		SCENELOG(Warning, "Tried to destroy a non-existing gameobject!");
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

	SCENELOG(Warning, "Could not find GameObject {} in scene!", aGameObject->GetName());
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

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(PipelineStateType::Shadow);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(mySpotLights[i]->GetComponent<Camera>());
		QueueGameObjects(mySpotLights[i]->GetComponent<Camera>());
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
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(PipelineStateType::ShadowCube);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(myPointLights[i]->GetComponent<Camera>());
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateShadowBuffer>(pointLight);
		QueueGameObjects(myPointLights[i]->GetComponent<Camera>(), true);
	}
}

void Scene::QueueDirectionalLightShadows()
{
	std::shared_ptr<DirectionalLight> dLight = myDirectionalLight->GetComponent<DirectionalLight>();
	if (!dLight->GetActive()) return;
	if (!dLight->CastsShadows()) return;
	
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetShadowMap(), false, true);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(PipelineStateType::Shadow);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(myDirectionalLight->GetComponent<Camera>());
	QueueGameObjects(myDirectionalLight->GetComponent<Camera>());
}

void Scene::QueueDebugGizmos(std::shared_ptr<Camera> aRenderCamera)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(PipelineStateType::Gizmo);
	CU::PlaneVolume<float> frustumVolume = aRenderCamera->GetFrustumPlaneVolume();

	for (auto& gameObject : myGameObjects)
	{
		std::shared_ptr<DebugModel> model = gameObject->GetComponent<DebugModel>();
		if (model && model->GetActive())
		{
			if (CU::IntersectionBetweenPlaneVolumeAABB(frustumVolume, model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderDebugMesh>(model);
			}
		}
	}
}

void Scene::QueueGameObjects(std::shared_ptr<Camera> aRenderCamera, bool disableViewCulling)
{
	for (auto& gameObject : myGameObjects)
	{
		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (disableViewCulling || !GraphicsEngine::Get().UseViewCulling || CU::IntersectionBetweenPlaneVolumeAABB(aRenderCamera->GetFrustumPlaneVolume(gameObject->GetComponent<Transform>()->GetWorldMatrix(true).GetFastInverse()), model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (disableViewCulling || !GraphicsEngine::Get().UseViewCulling || CU::IntersectionBetweenPlaneVolumeAABB(aRenderCamera->GetFrustumPlaneVolume(gameObject->GetComponent<Transform>()->GetWorldMatrix(true).GetFastInverse()), animModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel);
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