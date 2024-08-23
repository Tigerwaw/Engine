#include "Enginepch.h"
#include "Application.h"

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
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

#if _DEBUG
			extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
#endif

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

			Engine::GetInstance().GetInputHandler().UpdateEvents(msg.message, msg.wParam, msg.lParam);
		}

		Engine::GetInstance().GetImGuiHandler().BeginFrame();
		GraphicsEngine::Get().BeginFrame();
		Engine::GetInstance().GetImGuiHandler().Update();
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

bool Application::InitializeEngine()
{
	myWindow = std::make_unique<Window>();

	Engine& engine = Engine::GetInstance();
	std::string title = Engine::GetInstance().GetApplicationTitle();
	CU::Vector2f windowSize = Engine::GetInstance().GetWindowSize();
	bool fullscreen = Engine::GetInstance().GetIsFullscreen();
	bool borderless = Engine::GetInstance().GetIsBorderless();
	myWindow->InitializeWindow(title, windowSize, fullscreen, borderless);

	GraphicsEngine::Get().Initialize(myWindow->GetWindowHandle());
	CU::Vector2f resolution = engine.GetResolution();
	GraphicsEngine::Get().SetResolution(resolution.x, resolution.y);

	AssetManager::Get().Initialize(engine.GetContentRootPath());

#ifdef _DEBUG
	Engine::GetInstance().GetImGuiHandler().Initialize(myWindow->GetWindowHandle());
	GraphicsEngine::Get().InitializeImGui();
#endif

	LOG(LogApplication, Log, "Ready!");
	return true;
}