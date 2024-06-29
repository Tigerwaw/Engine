#pragma once
#include <memory>
#include <vector>
#include <string>
#include "GameEngine/UI/SpriteObject.h"

class GraphicsCommandList;
class GameObject;

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

	// TEMP
	void SetMainCamera(std::shared_ptr<GameObject> aCamera);
	std::shared_ptr<GameObject> GetMainCamera() { return myCamera; }
	void SetDirectionalLight(std::shared_ptr<GameObject> aDirectionalLight);
	std::shared_ptr<GameObject> GetDirectionalLight() { return myDirectionalLight; }
	void SetAmbientLight(std::shared_ptr<GameObject> aAmbientLight);
	std::shared_ptr<GameObject> GetAmbientLight() { return myAmbientLight; }
	void AddPointLight(std::shared_ptr<GameObject> aPointLight);
	std::shared_ptr<GameObject> GetPointLight(int aIndex) { return myPointLights[aIndex]; }
	void AddSpotLight(std::shared_ptr<GameObject> aSpotLight);
	std::shared_ptr<GameObject> GetSpotLight(int aIndex) { return mySpotLights[aIndex]; }

	std::vector<std::string> myDebugModeNames = {
		"None",
		"Unlit",
		"Wireframe",
		"DebugVertexNormals",
		"DebugPixelNormals",
		"DebugTextureNormals",
		"DebugUVs"
	};
	bool myShowGizmos = false;
private:
	void QueueClearTextureResources();
	void QueueShadowmapTextureResources();
	void QueueUpdateLightBuffer();
	void QueueSpotLightShadows();
	void QueuePointLightShadows();
	void QueueDirectionalLightShadows();
	void QueueDebugGizmos();
	void QueueGameObjects();
	std::shared_ptr<GraphicsCommandList> myCommandList;

	std::vector<std::shared_ptr<GameObject>> myGameObjects;
	unsigned myActiveGameObjectAmount = 0;

	// TEMP (?)
	std::shared_ptr<GameObject> myCamera;
	std::shared_ptr<GameObject> myAmbientLight;
	std::shared_ptr<GameObject> myDirectionalLight;
	std::vector<std::shared_ptr<GameObject>> myPointLights;
	std::vector<std::shared_ptr<GameObject>> mySpotLights;
	std::shared_ptr<SpriteObject> myTestSprite;
};

