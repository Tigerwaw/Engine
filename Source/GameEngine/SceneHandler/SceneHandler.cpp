#include "Enginepch.h"

#include "SceneHandler.h"
#include "SceneLoader.h"
#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"

#include "Engine.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneHandler, SceneHandler, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneHandler, SceneHandler, Warning);
#endif

#define SCENEHANDLERLOG(Verbosity, Message, ...) LOG(LogSceneHandler, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogSceneHandler);

SceneHandler::SceneHandler()
{
    mySceneLoader = std::make_shared<SceneLoader>();
}

void SceneHandler::UpdateActiveScene()
{
    if (!myActiveScene)
    {
        SCENEHANDLERLOG(Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myActiveScene->Update();
}

void SceneHandler::RenderActiveScene()
{
    if (!myActiveScene)
    {
        SCENEHANDLERLOG(Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myActiveScene->Render();
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
}

void SceneHandler::UnloadScene(unsigned aSceneIndex)
{
    if (myActiveScene == myLoadedScenes[aSceneIndex])
    {
        // bla bla bla unload stuff ig
    }

    myLoadedScenes.erase(myLoadedScenes.begin() + aSceneIndex);
    SCENEHANDLERLOG(Log, "Unloaded scene with index {}!", aSceneIndex);
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
        SCENEHANDLERLOG(Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myActiveScene->Instantiate(aGameObject);
}

void SceneHandler::Destroy(std::shared_ptr<GameObject> aGameObject)
{
    if (!myActiveScene)
    {
        SCENEHANDLERLOG(Error, "Scenehandler does not contain an active scene!");
        return;
    }

    myActiveScene->Destroy(aGameObject);
}

const unsigned SceneHandler::GetObjectAmount() const
{
    if (!myActiveScene)
    {
        SCENEHANDLERLOG(Error, "Scenehandler does not contain an active scene!");
        return 0;
    }

    return myActiveScene->GetObjectAmount();
}

const unsigned SceneHandler::GetActiveObjectAmount() const
{
    if (!myActiveScene)
    {
        SCENEHANDLERLOG(Error, "Scenehandler does not contain an active scene!");
        return 0;
    }

    return myActiveScene->GetActiveObjectAmount();
}
