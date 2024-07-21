#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/GlobalEventHandler/GlobalEventHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"

Engine::Engine()
{
    myTimer = std::make_unique<CU::Timer>();
    myInputHandler = std::make_unique<CU::InputHandler>();
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
