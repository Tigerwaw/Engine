#include "Enginepch.h"

#include "SceneLoader.h"

#include "ComponentSystem/Scene.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Component.h"

#include "AssetManager.h"
#include "Engine.h"
#include "Audio/AudioEngine.h"

#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/Model.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "ComponentSystem/Components/Graphics/VFXModel.h"
#include "ComponentSystem/Components/Graphics/Camera.h"
#include "ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "ComponentSystem/Components/Graphics/TrailSystem.h"

#include "ComponentSystem/Components/Lights/AmbientLight.h"
#include "ComponentSystem/Components/Lights/DirectionalLight.h"
#include "ComponentSystem/Components/Lights/PointLight.h"
#include "ComponentSystem/Components/Lights/SpotLight.h"

#include "ComponentSystem/Components/Physics/Colliders/BoxCollider.h"
#include "ComponentSystem/Components/Physics/Colliders/SphereCollider.h"

#include "ComponentSystem/Components/Movement/Rotator.h"
#include "ComponentSystem/Components/Movement/MoveBetweenPoints.h"
#include "ComponentSystem/Components/Movement/FreecamController.h"
#include "ComponentSystem/Components/AudioSource.h"

SceneLoader::SceneLoader() = default;
SceneLoader::~SceneLoader() = default;

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
        LOG(LogSceneLoader, Error, "Couldn't read scene file {}, {}!", aSceneFilepath.string(), e.what());
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

std::shared_ptr<GameObject> SceneLoader::LoadGameObject(std::shared_ptr<Scene> aScene, nl::json& aGO)
{
    std::shared_ptr<GameObject> newGO = std::make_shared<GameObject>();
    aScene->Instantiate(newGO);

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

    aScene->UpdateBoundingBox(newGO);

    if (aGO.contains("Children"))
    {
        for (auto& child : aGO["Children"])
        {
            std::shared_ptr<GameObject> newChild = LoadGameObject(aScene, child);
            if (newChild)
            {
                newGO->GetComponent<Transform>()->AddChild(newChild->GetComponent<Transform>().get());
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
        else if (comp == "VFXModel") newComponent = aGO->AddComponent<VFXModel>();
        else if (comp == "ParticleSystem") newComponent = aGO->AddComponent<ParticleSystem>();
        else if (comp == "TrailSystem") newComponent = aGO->AddComponent<TrailSystem>();
        else if (comp == "Camera") newComponent = aGO->AddComponent<Camera>();
        else if (comp == "AmbientLight") newComponent = aGO->AddComponent<AmbientLight>();
        else if (comp == "DirectionalLight") newComponent = aGO->AddComponent<DirectionalLight>();
        else if (comp == "PointLight") newComponent = aGO->AddComponent<PointLight>();
        else if (comp == "SpotLight") newComponent = aGO->AddComponent<SpotLight>();
        else if (comp == "Rotator") newComponent = aGO->AddComponent<Rotator>();
        else if (comp == "MoveBetweenPoints") newComponent = aGO->AddComponent<MoveBetweenPoints>();
        else if (comp == "FreecamController") newComponent = aGO->AddComponent<FreecamController>();
        else if (comp == "BoxCollider") newComponent = aGO->AddComponent<BoxCollider>();
        else if (comp == "SphereCollider") newComponent = aGO->AddComponent<SphereCollider>();

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
