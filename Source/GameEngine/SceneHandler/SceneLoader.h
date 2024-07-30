#pragma once
#include <memory>
#include <string>

#include <filesystem>
#include "nlohmann/json.hpp"
namespace nl = nlohmann;

class Scene;
class GameObject;

class SceneLoader
{
public:
    SceneLoader();
    ~SceneLoader();
    bool LoadScene(std::shared_ptr<Scene> aScene, std::filesystem::path aSceneFilepath);
    bool LoadGameObject(std::shared_ptr<Scene> aScene, nl::json& aGO);
    bool CreateComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
};

