#include "SceneLoader.h"
#include <iostream>
#include <fstream>

#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Component.h"

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneLoader, SceneLoader, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneLoader, SceneLoader, Warning);
#endif

#define SCENELOADERLOG(Verbosity, Message, ...) LOG(LogSceneLoader, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogSceneLoader);

SceneLoader::SceneLoader()
{

}

SceneLoader::~SceneLoader()
{

}

bool SceneLoader::LoadScene(std::shared_ptr<Scene> aScene, std::filesystem::path aSceneFilepath)
{
    if (!aScene)
    {
        aScene = std::make_shared<Scene>();
    }

    std::ifstream path(aSceneFilepath);
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        SCENELOADERLOG(Error, "Couldn't read scene file, {}!", e.what());
        return false;
    }
    path.close();

    if (data.contains("GameObjects"))
    {
        for (auto& gameObject : data["GameObjects"])
        {
            LoadGameObject(aScene, gameObject);
        }
    }

    return true;
}

bool SceneLoader::LoadGameObject(std::shared_ptr<Scene> aScene, nl::json& aGO)
{
    aScene;
    std::shared_ptr<GameObject> newGO = std::make_shared<GameObject>();

    if (aGO.contains("Name"))
    {
        newGO->SetName(aGO["Name"].get<std::string>());
        SCENELOADERLOG(Log, "Gameobject name: {}!", newGO->GetName());
    }
    
    return true;
}

bool SceneLoader::CreateComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    aGO;
    aComp;
    return true;
}
