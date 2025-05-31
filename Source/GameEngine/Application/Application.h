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

#include "GraphicsEngine.h"

#include "Engine.h"
#include "Input/InputHandler.h"
#include "SceneHandler/SceneHandler.h"
#include "DebugDrawer/DebugDrawer.h"
#include "Audio/AudioEngine.h"
#include "ImGui/ImGuiHandler.h"

#ifndef _RETAIL
#include "imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
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
private:

    bool myIsRunning = true;
    bool myIsPaused = false;
};

Application* CreateApplication();

