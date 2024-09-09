#include "Enginepch.h"

#include "Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/GlobalEventHandler/GlobalEventHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"
#include "GameEngine/ImGui/ImGuiHandler.h"
#include "GameEngine/Application/Application.h"

void Engine::Update()
{
    myDebugDrawer->ClearObjects();
    myTimer->Update();

    mySceneHandler->UpdateActiveScene();
    myInputHandler->UpdateInput();
    myAudioEngine->Update();
    mySceneHandler->RenderActiveScene();
}

WindowsEventHandler& Engine::GetWindowsEventHandler()
{
    return myApplicationInstance->GetWindowsEventHandler();
}

Window& Engine::GetApplicationWindow()
{
    return myApplicationInstance->GetWindow();
}

void Engine::LoadSettings(const std::string& aSettingsFilepath)
{
    std::ifstream path(aSettingsFilepath);
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        LOG(LogGameEngine, Error, "Couldn't read application settings file, {}!", e.what());
        return;
    }
    path.close();

    if (data.contains("title"))
    {
        std::string title = data["title"].get<std::string>();
        myTitle = title;
    }

    if (data.contains("assetsDir"))
    {
        std::filesystem::path assetsDir = data["assetsDir"].get<std::string>();
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

    if (data.contains("allowdropfiles"))
    {
        myAllowDropFiles = data["allowdropfiles"].get<bool>();
    }

    if (data.contains("autoregisterassets"))
    {
        myAutoRegisterAssets = data["autoregisterassets"].get<bool>();
    }
}

const std::filesystem::path Engine::GetContentRootPath()
{
    return myContentRoot;
}

const std::string& Engine::GetApplicationTitle()
{
    return myTitle;
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

void Engine::SetApplicationInstance(Application* aApplication)
{
    myApplicationInstance = aApplication;
}

void Engine::Destroy()
{
    myAudioEngine->Destroy();
    myImGuiHandler->Destroy();
}

Engine::Engine()
{
    LOG(LogGameEngine, Log, "Initializing Game Engine...");

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
    myIsBorderless = false;

    LOG(LogGameEngine, Log, "Game Engine initialized!");
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
