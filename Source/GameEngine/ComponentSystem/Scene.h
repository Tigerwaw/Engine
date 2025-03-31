#pragma once
#include "GameEngine/Intersections/AABB3D.hpp"
namespace CU = CommonUtilities;

class GameObject;
class Camera;
class Transform;
struct PipelineStateObject;

class Scene final
{
public:
	friend class SceneHandler;
	friend class SceneLoader;
	friend class Renderer;
	friend class CollisionHandler;

	Scene();
	~Scene();
	void Update();
	std::shared_ptr<GameObject> FindGameObjectByName(const std::string& aName);
	std::shared_ptr<GameObject> FindGameObjectByID(const unsigned aID);
	std::shared_ptr<GameObject> FindGameObjectByNetworkID(const unsigned aNetworkID);

	void Instantiate(std::shared_ptr<GameObject> aGameObject);
	void Destroy(std::shared_ptr<GameObject> aGameObject);

	const unsigned GetObjectAmount() const { return static_cast<unsigned>(myGameObjects.size()); }
	const unsigned GetActiveObjectAmount() const { return myActiveGameObjectAmount; }

	void SetActive(bool aIsActive) { myIsActive = aIsActive; }
	const bool GetActive() const { return myIsActive; }

private:
	void SortGameObjects();
	void DestroyInternal(GameObject* aGameObject);
	void DestroyHierarchy(Transform* aTransform);
	void UpdateBoundingBox(std::shared_ptr<GameObject> aGameObject);

	std::vector<std::shared_ptr<GameObject>> myGameObjects;
	std::vector<std::shared_ptr<GameObject>> myGameObjectsToDestroy;
	unsigned myActiveGameObjectAmount = 0;
	bool myIsActive = false;
	CU::AABB3D<float> myBoundingBox;

	unsigned myCurrentGameObjectID = 0;

	// TEMP (?)
	std::shared_ptr<GameObject> myMainCamera;
	std::shared_ptr<GameObject> myAmbientLight;
	std::shared_ptr<GameObject> myDirectionalLight;
	std::vector<std::shared_ptr<GameObject>> myPointLights;
	std::vector<std::shared_ptr<GameObject>> mySpotLights;
};
