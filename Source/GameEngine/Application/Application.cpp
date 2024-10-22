#include "Enginepch.h"
#include "Application.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application()
{
}

Application::~Application()
{
}

void Application::Run()
{
	if (!InitializeEngine())
	{
		return;
	}

	InitializeApplication();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

    while (myIsRunning)
    {
		PIXScopedEvent(PIX_COLOR_INDEX(0), "Main Update Loop");

#ifndef _RETAIL
		if (Engine::GetInstance().GetIsFullscreen())
		{
			if (ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam))
			{
				return;
			}
		}
#endif

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
			}

			if (msg.message == WM_ACTIVATE)
			{
				if (LOWORD(msg.wParam) == WA_INACTIVE)
				{
					myIsPaused = true;
				}
				else
				{
					myIsPaused = false;
				}
			}

			myEventHandler->HandleMessage(&msg);

			Engine::GetInstance().GetInputHandler().UpdateEvents(msg.message, msg.wParam, msg.lParam);
		}

		Engine::GetInstance().GetImGuiHandler().BeginFrame();
		GraphicsEngine::Get().BeginFrame();
		Engine::GetInstance().GetImGuiHandler().Update();
		Engine::GetInstance().GetDebugDrawer().ClearObjects();
		UpdateApplication();
		Engine::GetInstance().Update();
		GraphicsEngine::Get().RenderFrame();
		Engine::GetInstance().GetImGuiHandler().Render();
		GraphicsEngine::Get().EndFrame();
    }
}

void Application::Shutdown()
{
	Engine::GetInstance().Destroy();
}

Window& Application::GetWindow()
{
	return *myWindow;
}

WindowsEventHandler& Application::GetWindowsEventHandler()
{
	return *myEventHandler;
}

bool Application::InitializeEngine()
{
	myWindow = std::make_unique<Window>();
	myEventHandler = std::make_unique<WindowsEventHandler>();

	Engine& engine = Engine::GetInstance();
	std::string title = engine.GetApplicationTitle();
	CU::Vector2f windowSize = engine.GetWindowSize();
	bool fullscreen = engine.GetIsFullscreen();
	bool borderless = engine.GetIsBorderless();
	bool allowDropFiles = engine.GetAllowDropFiles();
	myWindow->InitializeWindow(title, windowSize, fullscreen, borderless, allowDropFiles);

	engine.SetApplicationInstance(this);

	GraphicsEngine::Get().Initialize(myWindow->GetWindowHandle());
	CU::Vector2f resolution = engine.GetResolution();
	GraphicsEngine::Get().SetResolution(resolution.x, resolution.y);

	AssetManager::Get().Initialize(engine.GetContentRootPath(), engine.GetAutoRegisterAssets());
	Engine::GetInstance().GetAudioEngine().Initialize();

#ifndef _RETAIL
	Engine::GetInstance().GetImGuiHandler().Initialize(myWindow->GetWindowHandle());
	GraphicsEngine::Get().InitializeImGui();
#endif

	LOG(LogApplication, Log, "Ready!");
	return true;
}