#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/GlobalEventHandler/GlobalEventHandler.h"

Engine::Engine()
{
    myTimer = std::make_shared<CU::Timer>();
    myInputHandler = std::make_shared<CU::InputHandler>();
    myGlobalEventHandler = std::make_shared<GlobalEventHandler>();

    myResolution = { 1920.0f, 1080.0f };
}

Engine::~Engine()
{
}
