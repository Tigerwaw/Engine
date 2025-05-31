#pragma once
#include "Math/Vector.hpp"


class Scene;
class GameObject;

class SceneLoader
{
public:
    SceneLoader();
    ~SceneLoader();
    bool LoadScene(std::shared_ptr<Scene> aScene, std::filesystem::path aSceneFilepath);
    std::shared_ptr<GameObject> LoadGameObject(std::shared_ptr<Scene> aScene, nl::json& aGO);
    void CreateComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
};

