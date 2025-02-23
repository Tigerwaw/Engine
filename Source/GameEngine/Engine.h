#pragma once
#include "Math/Vector.hpp"

namespace CU = CommonUtilities;

class GlobalEventHandler;
class SceneHandler;
class DebugDrawer;
class AudioEngine;
class InputHandler;
class Timer;
class ImGuiHandler;
class Application;
class WindowsEventHandler;
class Window;

class Engine
{
public:
    static Engine& GetInstance()
    {
        static Engine instance;
        return instance;
    }

    void Destroy();
    void Update();

    Timer& GetTimer() { return *myTimer; }
    InputHandler& GetInputHandler() { return *myInputHandler; }
    GlobalEventHandler& GetGlobalEventHandler() { return *myGlobalEventHandler; }
    SceneHandler& GetSceneHandler() { return *mySceneHandler; }
    DebugDrawer& GetDebugDrawer() { return *myDebugDrawer; }
    AudioEngine& GetAudioEngine() { return *myAudioEngine; }
    ImGuiHandler& GetImGuiHandler() { return *myImGuiHandler; }
    WindowsEventHandler& GetWindowsEventHandler();
    Window& GetApplicationWindow();

    void LoadSettings(const std::string& aSettingsFilepath);
    const std::filesystem::path GetContentRootPath();
    const std::string& GetApplicationTitle();

    void SetResolution(float aWidth, float aHeight);
    void SetWindowSize(float aWidth, float aHeight);
    void SetWindowPos(float aTop, float aLeft);
    void ToggleFullscreen(bool aIsFullscreen);
    const CU::Vector2f& GetResolution() const { return myResolution; }
    const CU::Vector2f& GetWindowSize() const { return myWindowSize; }
    const CU::Vector2f& GetWindowPos() const { return myWindowPos; }
    const bool& GetIsFullscreen() const { return myIsFullscreen; }
    const bool& GetIsBorderless() const { return myIsBorderless; }
    const bool& GetAllowDropFiles() const { return myAllowDropFiles; }
    const bool& GetAutoRegisterAssets() const { return myAutoRegisterAssets; }

    void SetApplicationInstance(Application* aApplication);

    // Temp kinda
    int RamUsage = 0;
    int RamUsageChange = 0;
    float TimeSinceLastMemoryCheck = 0;
    float MemoryCheckTimeInterval = 1.0f;
private:
    Engine();
    ~Engine();
    Engine(Engine const&) = delete;
    void operator=(Engine const&) = delete;
    static Engine* myInstance;

    std::unique_ptr<Timer> myTimer;
    std::unique_ptr<InputHandler> myInputHandler;
    std::unique_ptr<GlobalEventHandler> myGlobalEventHandler;
    std::unique_ptr<SceneHandler> mySceneHandler;
    std::unique_ptr<DebugDrawer> myDebugDrawer;
    std::unique_ptr<AudioEngine> myAudioEngine;
    std::unique_ptr<ImGuiHandler> myImGuiHandler;

    Application* myApplicationInstance = nullptr;

    std::string myTitle;
    std::filesystem::path myContentRoot;
    CU::Vector2f myResolution;
    CU::Vector2f myWindowSize;
    CU::Vector2f myWindowPos;
    bool myIsFullscreen = true;
    bool myIsBorderless = true;
    bool myAllowDropFiles = false;
    bool myAutoRegisterAssets = true;
};