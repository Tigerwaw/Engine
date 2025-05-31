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
#pragma endregion

#include "Math/Vector.hpp"


class Window
{
public:
    bool InitializeWindow(const std::string& aWindowTitle = "Application",
        Math::Vector2f aWindowSize = Math::Vector2f(1920.0f, 1080.0f), 
        Math::Vector2f aWindowPos = Math::Vector2f(0, 0),
        bool aIsFullscreen = true, bool aIsBorderless = true, 
        bool aAllowDropFiles = false);

    HWND GetWindowHandle() const { return myMainWindowHandle; }

    const Math::Vector2f GetCenter() const;
    const Math::Vector2f GetTopLeft() const;
    const Math::Vector2f GetTopRight() const;
    const Math::Vector2f GetBottomLeft() const;
    const Math::Vector2f GetBottomRight() const;
    const Math::Vector2f GetSize() const;
private:
    HWND myMainWindowHandle = nullptr;
};