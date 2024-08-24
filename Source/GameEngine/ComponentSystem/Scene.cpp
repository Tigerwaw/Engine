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

#include "GameEngine/Renderer/Renderer.h"
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
	myMainCamera = nullptr;
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

	UpdateBoundingBox(aGameObject);

	myGameObjects.emplace_back(aGameObject);

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
			myMainCamera = aGameObject;
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
			std::shared_ptr<Transform> goTransform = myGameObjects[i]->GetComponent<Transform>();
			if (goTransform)
			{
				DestroyHierarchy(goTransform.get());
			}

			return;
		}
	}

	LOG(LogScene, Warning, "Could not find GameObject {} in scene!", aGameObject->GetName());
}

void Scene::DestroyInternal(GameObject* aGameObject)
{
	for (size_t i = 0; i < myGameObjects.size(); i++)
	{
		if (myGameObjects[i].get() == aGameObject)
		{
			LOG(LogScene, Log, "Destroyed GameObject {}!", aGameObject->GetName());
			myGameObjects.erase(myGameObjects.begin() + i);
			return;
		}
	}
}

void Scene::DestroyHierarchy(Transform* aTransform)
{
	for (auto& child : aTransform->GetChildren())
	{
		DestroyHierarchy(child);
	}

	DestroyInternal(aTransform->gameObject);
}

void Scene::UpdateBoundingBox(std::shared_ptr<GameObject> aGameObject)
{
	if (aGameObject->GetComponent<Transform>())
	{
		CU::Vector3f bbMin = myBoundingBox.GetMin();
		CU::Vector3f bbMax = myBoundingBox.GetMax();
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

		myBoundingBox.InitWithMinAndMax(bbMin, bbMax);
	}
}