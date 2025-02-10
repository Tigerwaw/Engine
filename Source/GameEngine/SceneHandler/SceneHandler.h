#pragma once

class Scene;
class GameObject;
class SceneLoader;
class Renderer;
class CollisionHandler;

class SceneHandler
{
public:
    SceneHandler();
    ~SceneHandler();
    void UpdateActiveScene();
    void RenderActiveScene();
    void CreateEmptyScene();
    void LoadScene(std::string aSceneFilePath);
    void UnloadScene(unsigned aLoadedSceneIndex);
    void ChangeActiveScene(unsigned aLoadedSceneIndex);

    std::shared_ptr<GameObject> FindGameObjectByName(const std::string& aName);
    std::shared_ptr<GameObject> FindGameObjectByID(const unsigned aID);
    std::shared_ptr<GameObject> FindGameObjectByNetworkID(const unsigned aNetworkID);

    void Instantiate(std::shared_ptr<GameObject> aGameObject);
    void Destroy(std::shared_ptr<GameObject> aGameObject);

    bool Raycast(CU::Vector3f aOrigin, CU::Vector3f aDirection, CU::Vector3f& aHitPoint);

    const unsigned GetObjectAmount() const;
    const unsigned GetActiveObjectAmount() const;
private:
    std::unique_ptr<SceneLoader> mySceneLoader;
    std::unique_ptr<Renderer> myRenderer;
    std::unique_ptr<CollisionHandler> myCollisionHandler;

    std::shared_ptr<Scene> myActiveScene;
    std::vector<std::shared_ptr<Scene>> myLoadedScenes;
};

