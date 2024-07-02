#pragma once
#include <memory>
#include <string>
#include "Math/Vector.hpp"

namespace CommonUtilities
{
    class Timer;
    class InputHandler;
}

namespace CU = CommonUtilities;

class GlobalEventHandler;
class SceneHandler;

class Engine
{
public:
    static Engine& GetInstance()
    {
        static Engine instance;
        return instance;
    }

    CU::Timer& GetTimer() { return *myTimer; }
    CU::InputHandler& GetInputHandler() { return *myInputHandler; }
    GlobalEventHandler& GetGlobalEventHandler() { return *myGlobalEventHandler; }
    SceneHandler& GetSceneHandler() { return *mySceneHandler; }

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

    std::unique_ptr<CU::Timer> myTimer;
    std::unique_ptr<CU::InputHandler> myInputHandler;
    std::unique_ptr<GlobalEventHandler> myGlobalEventHandler;
    std::unique_ptr<SceneHandler> mySceneHandler;

    CU::Vector2f myResolution;
};