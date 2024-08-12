#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

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
private:
    void CreateTransformComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateModelComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateAnimatedModelComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateDebugModelComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateRotatorComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateFreecamControllerComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateAudioSourceComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateCameraComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateAmbientLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateDirectionalLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreatePointLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);
    void CreateSpotLightComponent(std::shared_ptr<GameObject> aGO, nl::json& aComp);

    CU::Vector3f GetVector3f(nl::json& aJSON);
};

