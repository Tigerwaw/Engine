#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/GlobalEventHandler/GlobalEventHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"
#include "GameEngine/ImGui/ImGuiHandler.h"

void Engine::Update()
{
    myDebugDrawer->ClearObjects();
    myTimer->Update();

    mySceneHandler->UpdateActiveScene();
    myImGuiHandler->Update();

    myInputHandler->UpdateInput();
    myAudioEngine->Update();
    mySceneHandler->RenderActiveScene();
    myDebugDrawer->DrawObjects();
}

void Engine::Destroy()
{
    myAudioEngine->Destroy();
    myImGuiHandler->Destroy();
}

Engine::Engine()
{
    myTimer = std::make_unique<Timer>();
    myInputHandler = std::make_unique<InputHandler>();
    myGlobalEventHandler = std::make_unique<GlobalEventHandler>();
    mySceneHandler = std::make_unique<SceneHandler>();
    myDebugDrawer = std::make_unique<DebugDrawer>();
    myAudioEngine = std::make_unique<AudioEngine>();
    myImGuiHandler = std::make_unique<ImGuiHandler>();

    myResolution = { 1920.0f, 1080.0f };
}

Engine::~Engine()
{
    myTimer = nullptr;
    myInputHandler = nullptr;
    myGlobalEventHandler = nullptr;
    mySceneHandler = nullptr;
    myDebugDrawer = nullptr;
    myAudioEngine = nullptr;
    myImGuiHandler = nullptr;
}
