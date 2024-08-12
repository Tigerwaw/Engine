#pragma once

class Scene;
class GameObject;
class SceneLoader;

class SceneHandler
{
public:
    SceneHandler();
    void UpdateActiveScene();
    void RenderActiveScene();
    void CreateEmptyScene();
    void LoadScene(std::string aSceneFilePath);
    void UnloadScene(unsigned aLoadedSceneIndex);
    void ChangeActiveScene(unsigned aLoadedSceneIndex);

    std::shared_ptr<GameObject> FindGameObjectByName(std::string aName);
    void Instantiate(std::shared_ptr<GameObject> aGameObject);
    void Destroy(std::shared_ptr<GameObject> aGameObject);

    const unsigned GetObjectAmount() const;
    const unsigned GetActiveObjectAmount() const;
private:
    std::shared_ptr<Scene> myActiveScene;
    std::vector<std::shared_ptr<Scene>> myLoadedScenes;
    std::shared_ptr<SceneLoader> mySceneLoader;
};

