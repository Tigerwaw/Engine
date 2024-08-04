#include "SceneLoader.h"
#include <iostream>
#include <fstream>

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
        if (aComp["Type"] == "Transform")
        {
            CreateTransformComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "AudioSource")
        {
            CreateAudioSourceComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "Model")
        {
            CreateModelComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "AnimatedModel")
        {
            CreateAnimatedModelComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "DebugModel")
        {
            CreateDebugModelComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "Camera")
        {
            CreateCameraComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "AmbientLight")
        {
            CreateAmbientLightComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "DirectionalLight")
        {
            CreateDirectionalLightComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "PointLight")
        {
            CreatePointLightComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "SpotLight")
        {
            CreateSpotLightComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "Rotator")
        {
            CreateRotatorComponent(aGO, aComp);
            return;
        }

        if (aComp["Type"] == "FreecamController")
        {
            CreateFreecamControllerComponent(aGO, aComp);
            return;
        }
    }

}

void SceneLoader::CreateTransformComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    CU::Vector3f pos;
    CU::Vector3f rot;
    CU::Vector3f scale = { 1.0f, 1.0f, 1.0f };

    if (aComp.contains("Position"))
    {
        pos = GetVector3f(aComp["Position"]);
    };

    if (aComp.contains("Rotation"))
    {
        rot = GetVector3f(aComp["Rotation"]);
    }

    if (aComp.contains("Scale"))
    {
        scale = GetVector3f(aComp["Scale"]);
    }

    aGO->AddComponent<Transform>(pos, rot, scale);
}

void SceneLoader::CreateModelComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    if (aComp.contains("Model"))
    {
        std::shared_ptr<Model> model = aGO->AddComponent<Model>();

        model->SetMesh(AssetManager::Get().GetAsset<MeshAsset>(aComp["Model"].get<std::string>())->mesh);

        if (aComp.contains("Materials"))
        {
            for (int i = 0; i < aComp["Materials"].size(); i++)
            {
                model->SetMaterialOnSlot(i, AssetManager::Get().GetAsset<MaterialAsset>(aComp["Materials"][i].get<std::string>())->material);
            }
        }
    }
}

void SceneLoader::CreateAnimatedModelComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    if (aComp.contains("Model"))
    {
        std::shared_ptr<AnimatedModel> model = aGO->AddComponent<AnimatedModel>();

        model->SetMesh(AssetManager::Get().GetAsset<MeshAsset>(aComp["Model"].get<std::string>())->mesh);

        if (aComp.contains("Materials"))
        {
            for (int i = 0; i < aComp["Materials"].size(); i++)
            {
                model->SetMaterialOnSlot(i, AssetManager::Get().GetAsset<MaterialAsset>(aComp["Materials"][i].get<std::string>())->material);
            }
        }

        if (aComp.contains("AnimationLayers"))
        {
            for (auto& layer : aComp["AnimationLayers"])
            {
                std::string startBone;

                if (layer.contains("StartBone"))
                {
                    startBone = layer["StartBone"].get<std::string>();
                }

                if (startBone != "")
                {
                    model->AddAnimationLayer(startBone);
                }

                if (layer.contains("Animations"))
                {
                    for (auto& animation : layer["Animations"])
                    {
                        std::string animationName;
                        std::shared_ptr<Animation> anim;
                        bool shouldLoop = false;

                        if (animation.contains("Name"))
                        {
                            animationName = animation["Name"].get<std::string>();
                        }

                        if (animation.contains("Path"))
                        {
                            anim = AssetManager::Get().GetAsset<AnimationAsset>(animation["Path"].get<std::string>())->animation;
                        }

                        if (animation.contains("Loop"))
                        {
                            shouldLoop = animation["Loop"].get<bool>();
                        }

                        model->AddAnimationToLayer(animationName, anim, startBone, shouldLoop);

                        if (animation.contains("Events"))
                        {
                            for (auto& animEvent : animation["Events"])
                            {
                                GameObjectEventType eventType = GameObjectEventType::Count;
                                unsigned eventFrame = 0;

                                if (animEvent.contains("EventType"))
                                {
                                    eventType = static_cast<GameObjectEventType>(animEvent["EventType"].get<int>());
                                }

                                if (animEvent.contains("Frame"))
                                {
                                    eventFrame = animEvent["Frame"].get<int>();
                                }

                                model->AddAnimationEvent(animationName, eventFrame, eventType, startBone);
                            }
                        }
                    }
                }
            }
        }
    }
}

void SceneLoader::CreateDebugModelComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    if (aComp.contains("Model"))
    {
        std::shared_ptr<DebugModel> model = aGO->AddComponent<DebugModel>();

        model->SetMesh(AssetManager::Get().GetAsset<MeshAsset>(aComp["Model"].get<std::string>())->mesh);
    }
}

void SceneLoader::CreateRotatorComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    std::shared_ptr<Rotator> rotator = aGO->AddComponent<Rotator>();

    if (aComp.contains("RotationPerSecond"))
    {
        rotator->SetRotationPerSecond(GetVector3f(aComp["RotationPerSecond"]));
    }
}

void SceneLoader::CreateFreecamControllerComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    float moveSpeed = 0;
    float rotSpeed = 0;

    if (aComp.contains("MoveSpeed"))
    {
        moveSpeed = aComp["MoveSpeed"].get<float>();
    }

    if (aComp.contains("RotationSpeed"))
    {
        rotSpeed = aComp["RotationSpeed"].get<float>();
    }

    std::shared_ptr<FreecamController> freecam = aGO->AddComponent<FreecamController>(moveSpeed, rotSpeed);
}

void SceneLoader::CreateAudioSourceComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    std::shared_ptr<AudioSource> audioSource = aGO->AddComponent<AudioSource>();

    std::string audioName = "";
    bool isOneShot = true;
    AudioSource::SourceType sourceType = AudioSource::SourceType::Non3D;
    bool playOnStart = false;

    if (aComp.contains("AudioName"))
    {
        audioName = aComp["AudioName"].get<std::string>();
    }

    if (aComp.contains("IsOneShot"))
    {
        isOneShot = aComp["IsOneShot"].get<bool>();
    }

    if (aComp.contains("SourceType"))
    {
        sourceType = static_cast<AudioSource::SourceType>(aComp["SourceType"].get<int>());
    }

    if (aComp.contains("PlayOnStart"))
    {
        playOnStart = aComp["PlayOnStart"].get<bool>();
    }

    audioSource->AddAudioInstance(audioName, isOneShot, sourceType, playOnStart);

    if (aComp.contains("PlayOnEvent"))
    {
        int eventInt = aComp["PlayOnEvent"].get<int>();

        if (eventInt < 0 || eventInt > static_cast<int>(GameObjectEventType::Count))
        {
            return;
        }

        audioSource->SetAudioPlayOnEvent(audioName, static_cast<GameObjectEventType>(eventInt));
    }
}

void SceneLoader::CreateCameraComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    if (aGO->GetName() == "MainCamera")
    {
        Engine::GetInstance().GetAudioEngine().SetListener(aGO);
    }

    if (aComp.contains("Perspective"))
    {
        bool isPerpective = aComp["Perspective"].get<bool>();
        if (isPerpective)
        {
            float fov = 0;
            float nearPlane = 0;
            float farPlane = 0;
            CU::Vector2f resolution = Engine::GetInstance().GetResolution();

            if (aComp.contains("FOV"))
            {
                fov = aComp["FOV"].get<float>();
            }

            if (aComp.contains("NearPlane"))
            {
                nearPlane = aComp["NearPlane"].get<float>();
            }

            if (aComp.contains("FarPlane"))
            {
                farPlane = aComp["FarPlane"].get<float>();
            }

            if (aComp.contains("Resolution"))
            {
                resolution = { aComp["Resolution"][0].get<float>(), aComp["Resolution"][1].get<float>() };
            }

            aGO->AddComponent<Camera>(fov, nearPlane, farPlane, resolution);
        }
        else
        {
            float leftPlane = 0;
            float rightPlane = 0;
            float topPlane = 0;
            float bottomPlane = 0;
            float nearPlane = 0;
            float farPlane = 0;

            if (aComp.contains("LeftPlane"))
            {
                leftPlane = aComp["LeftPlane"].get<float>();
            }

            if (aComp.contains("RightPlane"))
            {
                rightPlane = aComp["RightPlane"].get<float>();
            }

            if (aComp.contains("TopPlane"))
            {
                topPlane = aComp["TopPlane"].get<float>();
            }

            if (aComp.contains("BottomPlane"))
            {
                bottomPlane = aComp["BottomPlane"].get<float>();
            }

            if (aComp.contains("NearPlane"))
            {
                nearPlane = aComp["NearPlane"].get<float>();
            }

            if (aComp.contains("FarPlane"))
            {
                farPlane = aComp["FarPlane"].get<float>();
            }

            aGO->AddComponent<Camera>(leftPlane, rightPlane, topPlane, bottomPlane, nearPlane, farPlane);
        }
    }
}

void SceneLoader::CreateAmbientLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    if (aComp.contains("Cubemap"))
    {
        aGO->AddComponent<AmbientLight>(AssetManager::Get().GetAsset<TextureAsset>(aComp["Cubemap"].get<std::string>())->texture);
    }
}

void SceneLoader::CreateDirectionalLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    std::shared_ptr<DirectionalLight> dLight = aGO->AddComponent<DirectionalLight>();

    if (aComp.contains("Intensity"))
    {
        dLight->SetIntensity(aComp["Intensity"].get<float>());
    }

    if (aComp.contains("Color"))
    {
        dLight->SetColor(GetVector3f(aComp["Color"]));
    }

    if (aComp.contains("CastShadows") && aComp["CastShadows"].get<bool>())
    {
        unsigned shadowTextureSize = 512;
        float minShadowBias = 0.001f;
        float maxShadowBias = 0.005f;
        unsigned shadowSamples = 1;

        if (aComp.contains("ShadowTextureSize"))
        {
            shadowTextureSize = aComp["ShadowTextureSize"].get<unsigned>();
        }

        if (aComp.contains("MinShadowBias"))
        {
            minShadowBias = aComp["MinShadowBias"].get<float>();
        }

        if (aComp.contains("MaxShadowBias"))
        {
            maxShadowBias = aComp["MaxShadowBias"].get<float>();
        }

        if (aComp.contains("ShadowSamples"))
        {
            shadowSamples = aComp["ShadowSamples"].get<unsigned>();
        }

        dLight->EnableShadowCasting(shadowTextureSize, shadowTextureSize);
        dLight->SetShadowBias(minShadowBias, maxShadowBias);
        dLight->SetShadowSamples(shadowSamples);
    }
}

void SceneLoader::CreatePointLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    std::shared_ptr<PointLight> pLight = aGO->AddComponent<PointLight>();

    if (aComp.contains("Intensity"))
    {
        pLight->SetIntensity(aComp["Intensity"].get<float>());
    }

    if (aComp.contains("Color"))
    {
        pLight->SetColor(GetVector3f(aComp["Color"]));
    }

    if (aComp.contains("CastShadows") && aComp["CastShadows"].get<bool>())
    {
        unsigned shadowTextureSize = 512;
        float minShadowBias = 0.001f;
        float maxShadowBias = 0.005f;
        unsigned shadowSamples = 1;

        if (aComp.contains("ShadowTextureSize"))
        {
            shadowTextureSize = aComp["ShadowTextureSize"].get<unsigned>();
        }

        if (aComp.contains("MinShadowBias"))
        {
            minShadowBias = aComp["MinShadowBias"].get<float>();
        }

        if (aComp.contains("MaxShadowBias"))
        {
            maxShadowBias = aComp["MaxShadowBias"].get<float>();
        }

        if (aComp.contains("ShadowSamples"))
        {
            shadowSamples = aComp["ShadowSamples"].get<unsigned>();
        }

        pLight->EnableShadowCasting(shadowTextureSize, shadowTextureSize);
        pLight->SetShadowBias(minShadowBias, maxShadowBias);
        pLight->SetShadowSamples(shadowSamples);
    }
}

void SceneLoader::CreateSpotLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp)
{
    std::shared_ptr<SpotLight> sLight = aGO->AddComponent<SpotLight>();

    if (aComp.contains("Intensity"))
    {
        sLight->SetIntensity(aComp["Intensity"].get<float>());
    }

    if (aComp.contains("Color"))
    {
        sLight->SetColor(GetVector3f(aComp["Color"]));
    }

    if (aComp.contains("ConeAngle"))
    {
        sLight->SetConeAngle(aComp["ConeAngle"].get<float>());
    }

    if (aComp.contains("CastShadows") && aComp["CastShadows"].get<bool>())
    {
        unsigned shadowTextureSize = 512;
        float minShadowBias = 0.001f;
        float maxShadowBias = 0.005f;
        unsigned shadowSamples = 1;

        if (aComp.contains("ShadowTextureSize"))
        {
            shadowTextureSize = aComp["ShadowTextureSize"].get<unsigned>();
        }

        if (aComp.contains("MinShadowBias"))
        {
            minShadowBias = aComp["MinShadowBias"].get<float>();
        }

        if (aComp.contains("MaxShadowBias"))
        {
            maxShadowBias = aComp["MaxShadowBias"].get<float>();
        }

        if (aComp.contains("ShadowSamples"))
        {
            shadowSamples = aComp["ShadowSamples"].get<unsigned>();
        }

        sLight->EnableShadowCasting(shadowTextureSize, shadowTextureSize);
        sLight->SetShadowBias(minShadowBias, maxShadowBias);
        sLight->SetShadowSamples(shadowSamples);
    }
}

CU::Vector3f SceneLoader::GetVector3f(nl::json& aJSON)
{
    CU::Vector3f vector;
    
    if (aJSON.size() > 0)
    {
        vector.x = aJSON[0];
    }
    
    if (aJSON.size() > 1)
    {
        vector.y = aJSON[1];
    }
    
    if (aJSON.size() > 2)
    {
        vector.z = aJSON[2];
    }

    return vector;
}
