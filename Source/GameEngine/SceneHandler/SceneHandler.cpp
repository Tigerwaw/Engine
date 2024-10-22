#include "Enginepch.h"

#include "SceneHandler.h"
#include "SceneLoader.h"
#include "GameEngine/Renderer/Renderer.h"
#include "GameEngine/CollisionHandler/CollisionHandler.h"
#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"

#include "Engine.h"

SceneHandler::SceneHandler()
{
    mySceneLoader = std::make_unique<SceneLoader>();
    myRenderer = std::make_unique<Renderer>();
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

    myRenderer->RenderScene(*myActiveScene);
}

void SceneHandler::CreateEmptyScene()
{
    std::shared_ptr<Scene> newScene = myLoadedScenes.emplace_back(std::make_shared<Scene>());

    if (!myActiveScene)
    {
        myActiveScene = newScene;
    }
}

void SceneHandler::LoadScene(std::string aSceneFilePath)
{
    std::shared_ptr<Scene> newScene = myLoadedScenes.emplace_back(std::make_shared<Scene>());

    if (!myActiveScene)
    {
        myActiveScene = newScene;
    }

    mySceneLoader->LoadScene(newScene, Engine::GetInstance().GetContentRootPath() / aSceneFilePath);
    myRenderer->Init();
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

std::shared_ptr<GameObject> SceneHandler::FindGameObjectByName(std::string aName)
{
    return myActiveScene->FindGameObjectByName(aName);
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

bool SceneHandler::Raycast(CU::Vector3f aOrigin, CU::Vector3f aDirection, CU::Vector3f& aHitPoint)
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
