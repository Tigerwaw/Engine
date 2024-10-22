#pragma once
#pragma region WindowsIncludes
#define	WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS     
#define NOVIRTUALKEYCODES 
//#define NOWINMESSAGES     
//#define NOWINSTYLES       
//#define NOSYSMETRICS      
#define NOMENUS           
#define NOICONS           
#define NOKEYSTATES       
#define NOSYSCOMMANDS     
#define NORASTEROPS       
#define NOSHOWWINDOW      
#define OEMRESOURCE       
#define NOATOM            
#define NOCLIPBOARD       
#define NOCOLOR           
#define NOCTLMGR          
#define NODRAWTEXT        
#define NOGDI             
//#define NOKERNEL          
//#define NOUSER            
//#define NONLS
#define NOMB              
#define NOMEMMGR          
#define NOMETAFILE        
#define NOMINMAX          
//#define NOMSG             
#define NOOPENFILE        
#define NOSCROLL          
#define NOSERVICE         
#define NOSOUND           
#define NOTEXTMETRIC      
#define NOWH              
#define NOWINOFFSETS      
#define NOCOMM            
#define NOKANJI           
#define NOHELP            
#define NOPROFILER        
#define NODEFERWINDOWPOS  
#define NOMCX
#include <condition_variable>
#include <windows.h>
#include <wrl.h>
#include <shellapi.h>
#pragma endregion

#include <cstdio>
#include <exception>
#include <filesystem>
#include <future>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "Enginepch.h"

#include "AssetManager.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "Application/Window.h"
#include "Application/WindowsEventHandler.h"

#include "GameEngine/Engine.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"
#include "GameEngine/ImGui/ImGuiHandler.h"

#ifndef _RETAIL
#include "imgui.h"
#include "utilities/Imgui/misc/cpp/imgui_stdlib.h"
#define USE_PIX
#endif
#include "WinPixEventRuntime/pix3.h"


class Application
{
public:
    Application();
    virtual ~Application();

    virtual void InitializeApplication() {};
    virtual void UpdateApplication() {};
    void Run();
    void Shutdown();
    Window& GetWindow();
    WindowsEventHandler& GetWindowsEventHandler();
private:
    bool InitializeEngine();

    std::unique_ptr<Window> myWindow;
    std::unique_ptr<WindowsEventHandler> myEventHandler;

    bool myIsRunning = true;
    bool myIsPaused = false;
};

Application* CreateApplication();

