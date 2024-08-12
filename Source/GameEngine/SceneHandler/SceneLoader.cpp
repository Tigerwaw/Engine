#include "Enginepch.h"

#include "SceneLoader.h"

#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Component.h"

#include "AssetManager.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Audio/AudioEngine.h"

#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"

#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/SpotLight.h"


#include "GameEngine/ComponentSystem/Components/Movement/Rotator.h"
#include "GameEngine/ComponentSystem/Components/Movement/FreecamController.h"
#include "GameEngine/ComponentSystem/Components/AudioSource.h"

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
            std::shared_ptr<GameObject> newGO = LoadGameObject(aScene, gameObject);
            if (newGO)
            {
                aScene->Instantiate(newGO);
            }
        }
    }

    return true;
}

std::shared_ptr<GameObject> SceneLoader::LoadGameObject(std::shared_ptr<Scene> aScene, nl::json& aGO)
{
    std::shared_ptr<GameObject> newGO = std::make_shared<GameObject>();

    if (aGO.contains("Name"))
    {
        newGO->SetName(aGO["Name"].get<std::string>());
    }

    if (aGO.contains("Active"))
    {
        newGO->SetActive(aGO["Active"].get<bool>());
    }

    if (aGO.contains("Static"))
    {
        newGO->SetStatic(aGO["Static"].get<bool>());
    }

    if (aGO.contains("Components"))
    {
        for (auto& comp : aGO["Components"])
        {
            CreateComponent(newGO, comp);
        }
    }

    if (aGO.contains("Children"))
    {
        for (auto& child : aGO["Children"])
        {
            std::shared_ptr<GameObject> newChild = LoadGameObject(aScene, child);
            if (newChild)
            {
                newGO->GetComponent<Transform>()->AddChild(newChild->GetComponent<Transform>().get());
                aScene->Instantiate(newChild);
            }
        }
    }

    return newGO;
}

void SceneLoader::CreateComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    if (aComp.contains("Type"))
    {
        std::shared_ptr<Component> newComponent;

        if (aComp["Type"] == "Transform") newComponent = aGO->AddComponent<Transform>();
        else if (aComp["Type"] == "AudioSource") newComponent = aGO->AddComponent<AudioSource>();
        else if (aComp["Type"] == "Model") newComponent = aGO->AddComponent<Model>();
        else if (aComp["Type"] == "AnimatedModel") newComponent = aGO->AddComponent<AnimatedModel>();
        else if (aComp["Type"] == "Camera") newComponent = aGO->AddComponent<Camera>();
        else if (aComp["Type"] == "AmbientLight") newComponent = aGO->AddComponent<AmbientLight>();
        else if (aComp["Type"] == "DirectionalLight") newComponent = aGO->AddComponent<DirectionalLight>();
        else if (aComp["Type"] == "PointLight") newComponent = aGO->AddComponent<PointLight>();
        else if (aComp["Type"] == "SpotLight") newComponent = aGO->AddComponent<SpotLight>();
        else if (aComp["Type"] == "Rotator") newComponent = aGO->AddComponent<Rotator>();
        else if (aComp["Type"] == "FreecamController") newComponent = aGO->AddComponent<FreecamController>();

        if (newComponent)
        {
            newComponent->Deserialize(aComp);
        }
    }
}
