#include "Enginepch.h"
#include "Application.h"

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

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
		Engine::GetInstance().Update();
		Engine::GetInstance().GetImGuiHandler().Update();
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
	if (!InitializeWindow())
	{
		LOG(LogApplication, Log, "Failed to Initialize Main Window...");
		return false;
	}

	LOG(LogApplication, Log, "Initializing Graphics Engine...");

	if (!GraphicsEngine::Get().Initialize(myMainWindowHandle))
	{
		LOG(LogApplication, Log, "Failed to Initialize Graphics Engine...");
		return false;
	}

	CU::Vector2f resolution = Engine::GetInstance().GetResolution();
	GraphicsEngine::Get().SetResolution(resolution.x, resolution.y);

	AssetManager::Get().Initialize(Engine::GetInstance().GetContentRootPath());

#ifdef _DEBUG
	Engine::GetInstance().GetImGuiHandler().Initialize(myMainWindowHandle);
	GraphicsEngine::Get().InitializeImGui();
#endif

	ShowWindow(myMainWindowHandle, SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);

	LOG(LogApplication, Log, "Ready!");
	return true;
}

bool Application::InitializeWindow()
{
	SIZE windowSize = { static_cast<long>(Engine::GetInstance().GetWindowSize().x), static_cast<long>(Engine::GetInstance().GetWindowSize().y) };
	LPCWSTR windowTitle = L"ModelViewer";
	constexpr LPCWSTR windowClassName = L"ModelViewerMainWindow";

	if (Engine::GetInstance().GetIsFullscreen())
	{
		windowSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	}

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	LONG posX = (GetSystemMetrics(SM_CXSCREEN) - windowSize.cx) / 2;
	if (posX < 0)
		posX = 0;

	LONG posY = (GetSystemMetrics(SM_CYSCREEN) - windowSize.cy) / 2;
	if (posY < 0)
		posY = 0;

	long flags;

	if (Engine::GetInstance().GetIsBorderless())
	{
		flags = WS_POPUP;
	}
	else
	{
		flags = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_POPUP;
	}

	// Then we use the class to create our window
	myMainWindowHandle = CreateWindow(
		windowClassName,		// Classname
		windowTitle,			// Window Title
		flags,					// Flags
		posX,
		posY,
		windowSize.cx,
		windowSize.cy,
		nullptr, nullptr, nullptr,
		nullptr
	);

	return true;
}

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
	{
		PostQuitMessage(0);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}