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

#include "AssetTypes/MeshAsset.h"
#include "AssetTypes/NavMeshAsset.h"
#include "AssetTypes/AnimationAsset.h"
#include "AssetTypes/FontAsset.h"
#include "AssetTypes/MaterialAsset.h"
#include "AssetTypes/PSOAsset.h"
#include "AssetTypes/ShaderAsset.h"
#include "AssetTypes/TextureAsset.h"

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

    instance.RegisterDefaultAssetTypes();
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
    AssetManager::Get().Update();
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

void Engine::RegisterDefaultAssetTypes()
{
    AssetManager& am = AssetManager::Get();

    am.RegisterAssetType(".fbx", std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string& aFilename, const std::filesystem::path& aPath)
        {
            if (aFilename.starts_with("sm") || aFilename.starts_with("sk"))
            {
                return AssetManager::Get().RegisterAsset<MeshAsset>(aPath);
            }
            else if (aFilename.starts_with("a"))
            {
                return AssetManager::Get().RegisterAsset<AnimationAsset>(aPath);
            }
            else if (aFilename.starts_with("nm"))
            {
                return AssetManager::Get().RegisterAsset<NavMeshAsset>(aPath);
            }

            return false;
        }));

    am.RegisterAssetType(".mat", std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<MaterialAsset>(aPath);
        }));

    am.RegisterAssetType(".dds", std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<TextureAsset>(aPath);
        }));

    am.RegisterAssetType(".cso", std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<ShaderAsset>(aPath);
        }));

    am.RegisterAssetType(".pso", std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<PSOAsset>(aPath);
        }));

    am.RegisterAssetType(".font", std::function<bool(const std::string&, const std::filesystem::path&)>([](const std::string&, const std::filesystem::path& aPath)
        {
            return AssetManager::Get().RegisterAsset<FontAsset>(aPath);
        }));
}