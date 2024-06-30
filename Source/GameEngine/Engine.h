#pragma once
#include <memory>
#include "Math/Vector.hpp"

namespace CommonUtilities
{
    class Timer;
    class InputHandler;
}

namespace CU = CommonUtilities;

class GlobalEventHandler;

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

    const CU::Vector2f& GetResolution() const { return myResolution; }
private:
    Engine();
    ~Engine();
    Engine(Engine const&) = delete;
    void operator=(Engine const&) = delete;
    static Engine* myInstance;

    std::unique_ptr<CU::Timer> myTimer;
    std::unique_ptr<CU::InputHandler> myInputHandler;
    std::unique_ptr<GlobalEventHandler> myGlobalEventHandler;

    CU::Vector2f myResolution;
};