#include "Enginepch.h"

#include "Engine.h"
#include "Time/Timer.h"
#include "Input/InputHandler.h"
#include "GlobalEventHandler/GlobalEventHandler.h"
#include "SceneHandler/SceneHandler.h"
#include "DebugDrawer/DebugDrawer.h"
#include "Audio/AudioEngine.h"
#include "ImGui/ImGuiHandler.h"
#include "Application/Application.h"
#include "AssetManager.h"
#include "GraphicsEngine.h"
#include "Application/Window.h"
#include "Application/WindowsEventHandler.h"
#include "Application/AppSettings.h"

static Engine* sInstance = nullptr;

Engine& Engine::Get()
{
    assert(sInstance);
    return *sInstance;
}

void Engine::Initialize()
{
    assert(!sInstance);
    sInstance = new Engine();
    LOG(LogGameEngine, Log, "Initializing Game Engine...");

    Engine& instance = *sInstance;
    instance.myWindow = std::make_unique<Window>();
    instance.myEventHandler = std::make_unique<WindowsEventHandler>();
    instance.myTimer = std::make_unique<Timer>();
    instance.myInputHandler = std::make_unique<InputHandler>();
    instance.myGlobalEventHandler = std::make_unique<GlobalEventHandler>();
    instance.mySceneHandler = std::make_unique<SceneHandler>();
    instance.myDebugDrawer = std::make_unique<DebugDrawer>();
    instance.myAudioEngine = std::make_unique<AudioEngine>();
    instance.myImGuiHandler = std::make_unique<ImGuiHandler>();

    instance.myTitle = AppSettings::Get().title;
    instance.myContentRoot = AppSettings::Get().contentRoot;
    instance.myResolution = AppSettings::Get().resolution;
    instance.myWindowSize = AppSettings::Get().windowSize;
    instance.myWindowPos = AppSettings::Get().windowPos;
    instance.myIsFullscreen = AppSettings::Get().isFullscreen;
    instance.myIsBorderless = AppSettings::Get().isBorderless;
    instance.myAllowDropFiles = AppSettings::Get().allowDropFiles;
    instance.myAutoRegisterAssets = AppSettings::Get().autoRegisterAssets;
    AppSettings::Destroy();

    instance.myWindow->InitializeWindow(instance.myTitle, instance.myWindowSize, instance.myWindowPos, instance.myIsFullscreen, instance.myIsBorderless, instance.myAllowDropFiles);
    GraphicsEngine::Get().Initialize(instance.myWindow->GetWindowHandle(), instance.myResolution, instance.myContentRoot);
    GraphicsEngine::Get().SetResolution(instance.myResolution.x, instance.myResolution.y);
    GraphicsEngine::Get().SetWindowSize(instance.myWindowSize.x, instance.myWindowSize.y);

    AssetManager::Get().Initialize(instance.myContentRoot, instance.myAutoRegisterAssets);
    instance.myAudioEngine->Initialize();

#ifndef _RETAIL
    instance.myDebugDrawer->InitializeDebugDrawer();
    instance.myImGuiHandler->Initialize(instance.myWindow->GetWindowHandle());
    GraphicsEngine::Get().InitializeImGui();
#endif

    LOG(LogGameEngine, Log, "Game Engine initialized!");
}

void Engine::Shutdown()
{
    assert(sInstance);
    sInstance->myAudioEngine->Destroy();
    sInstance->myImGuiHandler->Destroy();

    delete sInstance;
    LOG(LogGameEngine, Log, "Shut down engine!");
}

void Engine::Prepare()
{
    myImGuiHandler->BeginFrame();
    GraphicsEngine::Get().BeginFrame();
    myDebugDrawer->ClearObjects();
}

void Engine::Update()
{
    myTimer->Update();
    mySceneHandler->UpdateActiveScene();
    myInputHandler->UpdateInput();
    myAudioEngine->Update();
    myImGuiHandler->Update();
    mySceneHandler->RenderActiveScene();
}

void Engine::Render()
{
    GraphicsEngine::Get().RenderFrame();
    myImGuiHandler->Render();
    GraphicsEngine::Get().EndFrame();
}

const std::filesystem::path& Engine::GetContentRootPath()
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

void Engine::SetWindowPos(float aTop, float aLeft)
{
    myWindowPos = { aTop, aLeft };
}

void Engine::ToggleFullscreen(bool aIsFullscreen)
{
    myIsFullscreen = aIsFullscreen;
}

Engine::Engine() = default;
Engine::~Engine() = default;
