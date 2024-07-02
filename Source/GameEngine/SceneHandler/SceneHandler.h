#pragma once
#include <memory>
#include <vector>
#include <string>

class Scene;
class GameObject;

class SceneHandler
{
public:
    void UpdateActiveScene();
    void RenderActiveScene();
    void CreateEmptyScene();
    void LoadScene();
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
};

