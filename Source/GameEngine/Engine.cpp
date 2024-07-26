#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/GlobalEventHandler/GlobalEventHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"

void Engine::Update()
{
    Engine::GetInstance().GetDebugDrawer().ClearObjects();
    Engine::GetInstance().GetTimer().Update();
    Engine::GetInstance().GetSceneHandler().UpdateActiveScene();
    Engine::GetInstance().GetInputHandler().UpdateInput();
    Engine::GetInstance().GetAudioEngine().Update();
    Engine::GetInstance().GetSceneHandler().RenderActiveScene();
    Engine::GetInstance().GetDebugDrawer().DrawObjects();
}

Engine::Engine()
{
    myTimer = std::make_unique<Timer>();
    myInputHandler = std::make_unique<InputHandler>();
    myGlobalEventHandler = std::make_unique<GlobalEventHandler>();
    mySceneHandler = std::make_unique<SceneHandler>();
    myDebugDrawer = std::make_unique<DebugDrawer>();
    myAudioEngine = std::make_unique<AudioEngine>();

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
}
