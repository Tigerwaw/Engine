#include "Enginepch.h"

#include "Scene.h"
#include "GraphicsEngine.h"
#include "Objects/Sprite.h"
#include "Objects/Material.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/Model.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "ComponentSystem/Components/Graphics/Camera.h"
#include "ComponentSystem/Components/Lights/AmbientLight.h"
#include "ComponentSystem/Components/Lights/DirectionalLight.h"
#include "ComponentSystem/Components/Lights/PointLight.h"
#include "ComponentSystem/Components/Lights/SpotLight.h"

#include "RenderAssembler/RenderAssembler.h"
#include "Engine.h"
#include "DebugDrawer/DebugDrawer.h"
#include "AssetManager.h"

#include "Math/Vector.hpp"

void Scene::Update()
{
	PIXScopedEvent(PIX_COLOR_INDEX(7), "Update GameObjects in Scene");
	myActiveGameObjectAmount = 0;

	for (auto& gameObject : myGameObjectsToDestroy)
	{
		if (auto goTransform = gameObject->GetComponent<Transform>())
		{
			DestroyHierarchy(goTransform.get());
		}
		else
		{
			DestroyInternal(gameObject.get());
		}
	}

	myGameObjectsToDestroy.clear();

	for (auto& gameObject : myGameObjects)
	{
		if (gameObject->GetActive())
		{
			gameObject->Update();
			myActiveGameObjectAmount++;
		}
	}

	SortGameObjects();
}

std::shared_ptr<GameObject> Scene::FindGameObjectByName(const std::string& aName)
{
	if (auto go = std::find_if(myGameObjects.begin(), myGameObjects.end(), [aName](const std::shared_ptr<GameObject>& object) { return object->GetName() == aName; }); *go != nullptr)
	{
		return *go;
	}

	LOG(LogScene, Warning, "Could not find game object with name {} in the scene!", aName);
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> Scene::FindGameObjectByID(const unsigned aID)
{
	if (auto go = std::find_if(myGameObjects.begin(), myGameObjects.end(), [aID](const std::shared_ptr<GameObject>& object) { return object->GetID() == aID; }); go != myGameObjects.end() && *go != nullptr)
	{
		return *go;
	}

	LOG(LogScene, Warning, "Could not find game object with ID {} in the scene!", aID);
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> Scene::FindGameObjectByNetworkID(const unsigned aNetworkID)
{
	if (auto go = std::find_if(myGameObjects.begin(), myGameObjects.end(), [aNetworkID](const std::shared_ptr<GameObject>& object) { return object->GetNetworkID() == aNetworkID; }); go != myGameObjects.end() && *go != nullptr)
	{
		return *go;
	}

	LOG(LogScene, Warning, "Could not find game object with network ID {} in the scene!", aNetworkID);
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

	aGameObject->SetID(myCurrentGameObjectID);
	myCurrentGameObjectID++;
	myGameObjects.emplace_back(aGameObject);

	LOG(LogScene, Log, "Created GameObject {}!", aGameObject->GetName());
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
			myGameObjectsToDestroy.push_back(aGameObject);
			return;
		}
	}

	LOG(LogScene, Warning, "Could not find GameObject {} in scene!", aGameObject->GetName());
}

void Scene::SortGameObjects()
{
	PIXScopedEvent(PIX_COLOR_INDEX(8), "Sort GameObjects in Scene");

	Math::Vector3f camPos;
	if (Camera::GetMainCamera())
		camPos = Camera::GetMainCamera()->gameObject->GetComponent<Transform>()->GetTranslation(true);

	std::stable_sort(myGameObjects.begin(), myGameObjects.end(), [this, camPos](const std::shared_ptr<GameObject> lhs, const std::shared_ptr<GameObject> rhs)
		{
			std::shared_ptr<Model> model1 = lhs->GetComponent<Model>();
			std::shared_ptr<Model> model2 = rhs->GetComponent<Model>();
			std::shared_ptr<AnimatedModel> animModel1 = lhs->GetComponent<AnimatedModel>();
			std::shared_ptr<AnimatedModel> animModel2 = rhs->GetComponent<AnimatedModel>();

			bool hasBlendState1 = false;
			bool hasBlendState2 = false;

			if (model1)
			{
				if (model1->GetMaterials().size() > 0)
					hasBlendState1 = model1->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr;
			}
			else if (animModel1)
			{
				if (animModel1->GetMaterials().size() > 0)
					hasBlendState1 = animModel1->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr;
			}

			if (model2)
			{
				if (model2->GetMaterials().size() > 0)
					hasBlendState2 = model2->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr;
			}
			else if (animModel2)
			{
				if (animModel2->GetMaterials().size() > 0)
					hasBlendState2 = animModel2->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr;
			}

			if (hasBlendState1 && !hasBlendState2)
			{
				return false;
			}
			else if (!hasBlendState1 && hasBlendState2)
			{
				return true;
			}
			else if (hasBlendState1 && hasBlendState2)
			{
				std::shared_ptr<Transform> transform1 = lhs->GetComponent<Transform>();
				std::shared_ptr<Transform> transform2 = rhs->GetComponent<Transform>();
				if (transform1 && transform2)
				{
					float distTo1 = Math::Vector3f(camPos - transform1->GetTranslation(true)).LengthSqr();
					float distTo2 = Math::Vector3f(camPos - transform2->GetTranslation(true)).LengthSqr();

					return distTo1 > distTo2;
				}
			}
			else
			{
				std::shared_ptr<Transform> transform1 = lhs->GetComponent<Transform>();
				std::shared_ptr<Transform> transform2 = rhs->GetComponent<Transform>();
				if (transform1 && transform2)
				{
					float distTo1 = Math::Vector3f(camPos - transform1->GetTranslation(true)).LengthSqr();
					float distTo2 = Math::Vector3f(camPos - transform2->GetTranslation(true)).LengthSqr();

					return distTo1 < distTo2;
				}
			}

			return false;
		});
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
		Math::Vector3f bbMin = myBoundingBox.GetMin();
		Math::Vector3f bbMax = myBoundingBox.GetMax();
		std::shared_ptr<Transform> objectTransform = aGameObject->GetComponent<Transform>();

		if (aGameObject->GetComponent<Model>())
		{
			auto& corners = aGameObject->GetComponent<Model>()->GetBoundingBox().GetCorners();

			for (Math::Vector3f corner : corners)
			{
				corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

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
			for (Math::Vector3f corner : corners)
			{
				corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

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
			Math::Vector3f point = objectTransform->GetTranslation(true);

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