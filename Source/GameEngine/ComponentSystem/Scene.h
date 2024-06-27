#pragma once
#include <memory>
#include <vector>
#include <string>

class GraphicsCommandList;
class GameObject;

enum class DebugMode
{
	None,
	Unlit,
	Wireframe,
	DebugVertexNormals,
	DebugPixelNormals,
	DebugTextureNormals,
	DebugUVs
};

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

	DebugMode myCurrentDebugMode = DebugMode::None;
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
	void DebugRender();
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

	// TEMP (?)
	std::shared_ptr<GameObject> myCamera;
	std::shared_ptr<GameObject> myAmbientLight;
	std::shared_ptr<GameObject> myDirectionalLight;
	std::vector<std::shared_ptr<GameObject>> myPointLights;
	std::vector<std::shared_ptr<GameObject>> mySpotLights;
};

