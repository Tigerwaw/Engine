#pragma once
#include <memory>
#include <vector>
#include <string>

#include "GameEngine/Intersections/AABB3D.hpp"

class GameObject;
class SceneHandler;
class Camera;

class Scene final
{
public:
	Scene();
	~Scene();
	void Update();
	void Render();
	std::shared_ptr<GameObject> FindGameObjectByName(std::string aName);
	void Instantiate(std::shared_ptr<GameObject> aGameObject);
	void Destroy(std::shared_ptr<GameObject> aGameObject);

	const unsigned GetObjectAmount() const { return static_cast<unsigned>(myGameObjects.size()); }
	const unsigned GetActiveObjectAmount() const { return myActiveGameObjectAmount; }

	void SetActive(bool aIsActive) { myIsActive = aIsActive; }
	const bool GetActive() const { return myIsActive; }

private:
	void QueueClearTextureResources();
	void QueueShadowmapTextureResources();
	void QueueUpdateLightBuffer();
	void QueueSpotLightShadows();
	void QueuePointLightShadows();
	void QueueDirectionalLightShadows();
	void QueueDebugGizmos(std::shared_ptr<Camera> aRenderCamera);
	void QueueGameObjects(std::shared_ptr<Camera> aRenderCamera);

	std::vector<std::shared_ptr<GameObject>> myGameObjects;
	unsigned myActiveGameObjectAmount = 0;
	bool myIsActive = false;

	// TEMP (?)
	std::shared_ptr<GameObject> myCamera;
	std::shared_ptr<GameObject> myAmbientLight;
	std::shared_ptr<GameObject> myDirectionalLight;
	std::vector<std::shared_ptr<GameObject>> myPointLights;
	std::vector<std::shared_ptr<GameObject>> mySpotLights;
};
