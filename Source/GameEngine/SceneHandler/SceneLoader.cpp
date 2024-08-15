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
        LOG(LogSceneLoader, Error, "Couldn't read scene file, {}!", e.what());
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
        std::string comp = aComp["Type"];

        if (comp == "Transform") newComponent = aGO->AddComponent<Transform>();
        else if (comp == "AudioSource") newComponent = aGO->AddComponent<AudioSource>();
        else if (comp == "Model") newComponent = aGO->AddComponent<Model>();
        else if (comp == "AnimatedModel") newComponent = aGO->AddComponent<AnimatedModel>();
        else if (comp == "Camera") newComponent = aGO->AddComponent<Camera>();
        else if (comp == "AmbientLight") newComponent = aGO->AddComponent<AmbientLight>();
        else if (comp == "DirectionalLight") newComponent = aGO->AddComponent<DirectionalLight>();
        else if (comp == "PointLight") newComponent = aGO->AddComponent<PointLight>();
        else if (comp == "SpotLight") newComponent = aGO->AddComponent<SpotLight>();
        else if (comp == "Rotator") newComponent = aGO->AddComponent<Rotator>();
        else if (comp == "FreecamController") newComponent = aGO->AddComponent<FreecamController>();

        if (newComponent)
        {
            newComponent->Deserialize(aComp);
        }
        else
        {
            LOG(LogSceneLoader, Warning, "Could not create component of type {} for gameobject {}", comp, aGO->GetName());
        }
    }
    else
    {
        LOG(LogSceneLoader, Warning, "Could not find type of component to add to gameobject {}", aGO->GetName());
    }
}
