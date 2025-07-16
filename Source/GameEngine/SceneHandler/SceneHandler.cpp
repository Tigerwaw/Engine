#include "Enginepch.h"

#include "SceneHandler.h"
#include "SceneLoader.h"
#include "RenderAssembler/RenderAssembler.h"
#include "CollisionHandler/CollisionHandler.h"
#include "ComponentSystem/Scene.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Graphics/Camera.h"

#include "Engine.h"

SceneHandler::SceneHandler()
{
    mySceneLoader = std::make_unique<SceneLoader>();
    myRenderAssembler = std::make_unique<RenderAssembler>();
    myCollisionHandler = std::make_unique<CollisionHandler>();
}

SceneHandler::~SceneHandler()
{
}

void SceneHandler::UpdateActiveScene()
{
    PIXScopedEvent(PIX_COLOR_INDEX(3), "Update Active Scene");
    if (!myActiveScene)
    {
        LOG(LogSceneHandler, Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myActiveScene->Update();
    myCollisionHandler->TestCollisions(*myActiveScene);
}

void SceneHandler::RenderActiveScene()
{
    if (!myActiveScene)
    {
        LOG(LogSceneHandler, Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myRenderAssembler->RenderScene(*myActiveScene);
}

void SceneHandler::CreateEmptyScene()
{
    std::shared_ptr<Scene> newScene = myLoadedScenes.emplace_back(std::make_shared<Scene>());

    if (!myActiveScene)
    {
        myActiveScene = newScene;
    }
}

void SceneHandler::LoadScene(const std::string& aSceneFilePath)
{
    std::shared_ptr<Scene> newScene = myLoadedScenes.emplace_back(std::make_shared<Scene>());

    if (!myActiveScene)
    {
        myActiveScene = newScene;
    }

    mySceneLoader->LoadScene(newScene, Engine::Get().GetContentRootPath() / aSceneFilePath);
    myRenderAssembler->Init();
}

void SceneHandler::UnloadScene(unsigned aSceneIndex)
{
    if (myActiveScene == myLoadedScenes[aSceneIndex])
    {
        // bla bla bla unload stuff ig
    }

    myLoadedScenes.erase(myLoadedScenes.begin() + aSceneIndex);
    LOG(LogSceneHandler, Log, "Unloaded scene with index {}!", aSceneIndex);
}

void SceneHandler::ChangeActiveScene(unsigned aSceneIndex)
{
    myActiveScene = myLoadedScenes[aSceneIndex];
}

void SceneHandler::SetMainCamera(std::shared_ptr<Camera> aCamera)
{
    aCamera->SetAsMainCamera();
}

std::shared_ptr<GameObject> SceneHandler::FindGameObjectByName(const std::string& aName)
{
    return myActiveScene->FindGameObjectByName(aName);
}

std::shared_ptr<GameObject> SceneHandler::FindGameObjectByID(const unsigned aID)
{
    return myActiveScene->FindGameObjectByID(aID);
}

std::shared_ptr<GameObject> SceneHandler::FindGameObjectByNetworkID(const unsigned aNetworkID)
{
    return myActiveScene->FindGameObjectByNetworkID(aNetworkID);
}

void SceneHandler::Instantiate(std::shared_ptr<GameObject> aGameObject)
{
    if (!myActiveScene)
    {
        LOG(LogSceneHandler, Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myActiveScene->Instantiate(aGameObject);
}

void SceneHandler::Destroy(std::shared_ptr<GameObject> aGameObject)
{
    if (!myActiveScene)
    {
        LOG(LogSceneHandler, Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myActiveScene->Destroy(aGameObject);
}

bool SceneHandler::Raycast(Math::Vector3f aOrigin, Math::Vector3f aDirection, Math::Vector3f& aHitPoint)
{
    return myCollisionHandler->Raycast(*myActiveScene, aOrigin, aDirection, aHitPoint);
}

const unsigned SceneHandler::GetObjectAmount() const
{
    if (!myActiveScene)
    {
        LOG(LogSceneHandler, Error, "Scenehandler does not contain an active scene!");
        return 0;
    }

    return myActiveScene->GetObjectAmount();
}

const unsigned SceneHandler::GetActiveObjectAmount() const
{
    if (!myActiveScene)
    {
        LOG(LogSceneHandler, Error, "Scenehandler does not contain an active scene!");
        return 0;
    }

    return myActiveScene->GetActiveObjectAmount();
}
