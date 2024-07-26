#pragma once
#include <memory>
#include <string>
#include "Math/Vector.hpp"

namespace CU = CommonUtilities;

class GlobalEventHandler;
class SceneHandler;
class DebugDrawer;
class AudioEngine;
class InputHandler;
class Timer;

class Engine
{
public:
    static Engine& GetInstance()
    {
        static Engine instance;
        return instance;
    }

    void Update();

    Timer& GetTimer() { return *myTimer; }
    InputHandler& GetInputHandler() { return *myInputHandler; }
    GlobalEventHandler& GetGlobalEventHandler() { return *myGlobalEventHandler; }
    SceneHandler& GetSceneHandler() { return *mySceneHandler; }
    DebugDrawer& GetDebugDrawer() { return *myDebugDrawer; }
    AudioEngine& GetAudioEngine() { return *myAudioEngine; }

    const CU::Vector2f& GetResolution() const { return myResolution; }

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

    CU::Vector2f myResolution;
};