#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/GlobalEventHandler/GlobalEventHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"
#include "GameEngine/ImGui/ImGuiHandler.h"

#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
namespace nl = nlohmann;

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, GameEngine, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, GameEngine, Warning);
#endif

#define ENGINELOG(Verbosity, Message, ...) LOG(LogGameEngine, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogGameEngine);

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

const std::filesystem::path Engine::GetContentRootPath()
{
    return myContentRoot;
}

void Engine::SetResolution(float aWidth, float aHeight)
{
    myResolution = { aWidth, aHeight };
}

void Engine::SetWindowSize(float aWidth, float aHeight)
{
    myWindowSize = { aWidth, aHeight };
}

void Engine::ToggleFullscreen(bool aIsFullscreen)
{
    myIsFullscreen = aIsFullscreen;
}

void Engine::Destroy()
{
    myAudioEngine->Destroy();
    myImGuiHandler->Destroy();
}

Engine::Engine()
{
    ENGINELOG(Log, "Initializing Game Engine...");

    myTimer = std::make_unique<Timer>();
    myInputHandler = std::make_unique<InputHandler>();
    myGlobalEventHandler = std::make_unique<GlobalEventHandler>();
    mySceneHandler = std::make_unique<SceneHandler>();
    myDebugDrawer = std::make_unique<DebugDrawer>();
    myAudioEngine = std::make_unique<AudioEngine>();
    myImGuiHandler = std::make_unique<ImGuiHandler>();

    myResolution = { 1920.0f, 1080.0f };
    myWindowSize = { 1920.0f, 1080.0f };
    myIsFullscreen = false;

    std::filesystem::path exeDir = std::filesystem::current_path();
    std::ifstream path(exeDir / "ApplicationSettings.json");
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        ENGINELOG(Error, "Couldn't read application settings file, {}!", e.what());
        return;
    }
    path.close();

    if (data.contains("assetsDir"))
    {
        std::filesystem::path assetsDir = exeDir / data["assetsDir"].get<std::string>();
        myContentRoot = assetsDir;
    }

    if (data.contains("resolution"))
    {
        myResolution = { data["resolution"]["width"].get<float>(), data["resolution"]["height"].get<float>() };
    }

    if (data.contains("windowSize"))
    {
        myWindowSize = { data["windowSize"]["width"].get<float>(), data["windowSize"]["height"].get<float>() };
    }

    if (data.contains("fullscreen"))
    {
        myIsFullscreen = data["fullscreen"].get<bool>();
    }

    if (data.contains("borderless"))
    {
        myIsBorderless = data["borderless"].get<bool>();
    }

    ENGINELOG(Log, "Game Engine initialized!");
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
