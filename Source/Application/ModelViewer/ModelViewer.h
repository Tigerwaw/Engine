#pragma once
#include <memory>
#include <vector>
#include "GameEngine/ComponentSystem/Scene.h"
#include "Graphics\GraphicsEngine\GraphicsEngine.h"

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogModelViewer, ModelViewer, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogModelViewer, ModelViewer, Warning);
#endif

#define MVLOG(Verbosity, Message, ...) LOG(LogModelViewer, Verbosity, Message, ##__VA_ARGS__)

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
#include <Windows.h>
#pragma endregion

struct Animation;
class GameObject;

namespace CommonUtilities
{
	class Timer;
}

class ModelViewer
{
public:
	ModelViewer();
	void Shutdown();

	bool Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle);
	void InitModelViewer();
	int Run();

private:
	void Update();
	void Render();
	void InitCamera();
	void InitLights();
	void InitGameObjects();
	void ChangeAnimation(int aIndex);
	void SetupImguiStyle();
	void UpdateImgui();

	bool myIsRunning = false;
	HWND myMainWindowHandle = nullptr;

	Scene myScene;

	int ramUsage = 0;
	int ramUsageChange = 0;
	float timeSinceLastMemoryCheck = 0;
	float memoryCheckTimeInterval = 1.0f;

	std::shared_ptr<GameObject> myTgaBro;
	std::vector<std::shared_ptr<Animation>> myAnimations;
	std::vector<std::string> myAnimationNames;
	unsigned myCurrentAnimIndex = 0;
};
