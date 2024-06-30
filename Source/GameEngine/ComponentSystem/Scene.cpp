#include "Scene.h"
#include "GraphicsEngine/RHI/GraphicsCommands/GraphicsCommandList.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/SpotLight.h"

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
	myCommandList = std::make_shared<GraphicsCommandList>();
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
	myCommandList->Enqueue<ChangePipelineState>(PipelineStateType::Default);
	myCommandList->Enqueue<SetDefaultRenderTarget>();
	myCommandList->Enqueue<UpdateFrameBuffer>(myCamera->GetComponent<Camera>());
	myCommandList->Enqueue<SetTextureResource>(126, myAmbientLight->GetComponent<AmbientLight>()->GetEnvironmentTexture());
	QueueShadowmapTextureResources();

	QueueGameObjects();
	myCommandList->Enqueue<ChangePipelineState>(PipelineStateType::Gizmo);

	if (myShowGizmos)
	{
		QueueDebugGizmos();
	}
	
	QueueClearTextureResources();

	// Run Command List
	if (myCommandList->HasCommands() && !myCommandList->IsFinished())
	{
		myCommandList->Execute();
	}
	myCommandList->Reset();
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
	myGameObjects.emplace_back(aGameObject);
}

void Scene::Destroy(std::shared_ptr<GameObject> aGameObject)
{
	if (!aGameObject)
	{
		SCENELOG(Warning, "Tried to destroy non-existing gameobject!");
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

void Scene::SetMainCamera(std::shared_ptr<GameObject> aCamera)
{
	std::shared_ptr<GameObject> camera = myGameObjects.emplace_back(aCamera);
	myCamera = camera;
}

void Scene::SetAmbientLight(std::shared_ptr<GameObject> aAmbientLight)
{
	std::shared_ptr<GameObject> aLight = myGameObjects.emplace_back(aAmbientLight);
	myAmbientLight = aLight;
}

void Scene::SetDirectionalLight(std::shared_ptr<GameObject> aDirectionalLight)
{
	std::shared_ptr<GameObject> dLight = myGameObjects.emplace_back(aDirectionalLight);
	myDirectionalLight = dLight;
}

void Scene::AddPointLight(std::shared_ptr<GameObject> aPointLight)
{
	std::shared_ptr<GameObject> pLight = myGameObjects.emplace_back(aPointLight);
	myPointLights.emplace_back(pLight);
}

void Scene::AddSpotLight(std::shared_ptr<GameObject> aSpotLight)
{
	std::shared_ptr<GameObject> sLight = myGameObjects.emplace_back(aSpotLight);
	mySpotLights.emplace_back(sLight);
}

void Scene::QueueClearTextureResources()
{
	for (int i = 100; i < 110; i++)
	{
		myCommandList->Enqueue<ClearTextureResource>(i);
	}
	myCommandList->Enqueue<ClearTextureResource>(126);
}

void Scene::QueueShadowmapTextureResources()
{
	myCommandList->Enqueue<SetTextureResource>(100, myDirectionalLight->GetComponent<DirectionalLight>()->GetShadowMap());

	for (int i = 0; i < myPointLights.size(); i++)
	{
		if (i >= MAX_POINTLIGHTS) break;

		std::shared_ptr<PointLight> pLight = myPointLights[i]->GetComponent<PointLight>();
		if (!pLight->GetActive()) continue;
		if (!pLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = pLight->GetShadowMap();
		if (!shadowMap) continue;

		myCommandList->Enqueue<SetTextureResource>(101 + i, shadowMap);
	}

	for (int i = 0; i < mySpotLights.size(); i++)
	{
		if (i >= MAX_SPOTLIGHTS) break;

		std::shared_ptr<SpotLight> sLight = mySpotLights[i]->GetComponent<SpotLight>();
		if (!sLight->GetActive()) continue;
		if (!sLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = sLight->GetShadowMap();
		if (!shadowMap) continue;

		myCommandList->Enqueue<SetTextureResource>(105 + i, shadowMap);
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

	myCommandList->Enqueue<UpdateLightBuffer>(ambientLight, dirLight, pointLights, spotLights);
}

void Scene::QueueSpotLightShadows()
{
	for (int i = 0; i < mySpotLights.size(); i++)
	{
		std::shared_ptr<SpotLight> spotLight = mySpotLights[i]->GetComponent<SpotLight>();
		if (!spotLight->GetActive()) continue;
		if (!spotLight->CastsShadows()) continue;

		myCommandList->Enqueue<ChangePipelineState>(PipelineStateType::Shadow);
		myCommandList->Enqueue<SetRenderTarget>(nullptr, spotLight->GetShadowMap(), false, true);
		myCommandList->Enqueue<UpdateFrameBuffer>(mySpotLights[i]->GetComponent<Camera>());
		QueueGameObjects();
	}
}

void Scene::QueuePointLightShadows()
{
	for (int i = 0; i < myPointLights.size(); i++)
	{
		std::shared_ptr<PointLight> pointLight = myPointLights[i]->GetComponent<PointLight>();
		if (!pointLight->GetActive()) continue;
		if (!pointLight->CastsShadows()) continue;
		
		myCommandList->Enqueue<SetRenderTarget>(nullptr, pointLight->GetShadowMap(), false, true);
		myCommandList->Enqueue<ChangePipelineState>(PipelineStateType::ShadowCube);
		myCommandList->Enqueue<UpdateFrameBuffer>(myPointLights[i]->GetComponent<Camera>());
		myCommandList->Enqueue<UpdateShadowBuffer>(pointLight);
		QueueGameObjects();
	}
}

void Scene::QueueDirectionalLightShadows()
{
	std::shared_ptr<DirectionalLight> dLight = myDirectionalLight->GetComponent<DirectionalLight>();
	if (!dLight->GetActive()) return;
	if (!dLight->CastsShadows()) return;
	
	myCommandList->Enqueue<SetRenderTarget>(nullptr, dLight->GetShadowMap(), false, true);
	myCommandList->Enqueue<ChangePipelineState>(PipelineStateType::Shadow);
	myCommandList->Enqueue<UpdateFrameBuffer>(myDirectionalLight->GetComponent<Camera>());
	QueueGameObjects();
}

void Scene::QueueDebugGizmos()
{
	for (auto& gameObject : myGameObjects)
	{
		std::shared_ptr<DebugModel> model = gameObject->GetComponent<DebugModel>();
		if (model && model->GetActive())
		{
			myCommandList->Enqueue<RenderDebugMesh>(model);
		}
	}
}

void Scene::QueueGameObjects()
{
	for (auto& gameObject : myGameObjects)
	{
		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			myCommandList->Enqueue<RenderMesh>(model);
		}


		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			myCommandList->Enqueue<RenderAnimatedMesh>(animModel);
		}
	}
}