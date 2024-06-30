#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/GlobalEventHandler/GlobalEventHandler.h"

Engine::Engine()
{
    myTimer = std::make_unique<CU::Timer>();
    myInputHandler = std::make_unique<CU::InputHandler>();
    myGlobalEventHandler = std::make_unique<GlobalEventHandler>();

    myResolution = { 1920.0f, 1080.0f };
}

Engine::~Engine()
{
    myTimer = nullptr;
    myInputHandler = nullptr;
    myGlobalEventHandler = nullptr;
}
