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
#ifdef _DEBUG
	// Redirect stdout and stderr to the console.
	FILE* consoleOut;
	FILE* consoleErr;
	AllocConsole();
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);  // NOLINT(cert-err33-c)
	setvbuf(consoleOut, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)
	freopen_s(&consoleErr, "CONOUT$", "w", stderr);  // NOLINT(cert-err33-c)
	setvbuf(consoleErr, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)

	/*
	 * A note about strings:
	 * Strings in C++ are terrible things. They come in a variety of formats which causes issues.
	 * Many modern libraries expect UTF-8 (ImGui, FMOD, FBX SDK, etc) but Windows defaults to UTF-16LE
	 * which is not compatible 1=1.
	 *
	 * To avoid weird problems with compatibility, mangled characters and other problems it is highly
	 * recommended to store everything as UTF-8 encoded strings whenever possible. This means that
	 * when we store i.e. the name of a Model, Level, Sound File, etc we do so in a UTF-8 formatted
	 * std::string, and when we need to communicate with the Windows (or DirectX) API we need to use
	 * std::wstring which represents a UTF-16LE string.
	 *
	 * There are functions available for conversion between these formats in the str namespace in
	 * StringHelpers.h.
	 *
	 * The provided Logging library expects UTF-8 format strings which should provide minimal headaches
	 * for any involved situation. For anything non-unicode (like non swedish signs, accents, etc) you
	 * can just use normal strings as you would anywhere since UTF-8 is backwards compatible with ASCii
	 *
	 * SetConsoleOutputCP(CP_UTF8) tells the Windows Console that we'll output UTF-8. This DOES NOT
	 * affect file output in any way, that's a whole other can of worms. But if you always write and
	 * read your strings in the same format, and always treat them as byte blocks, you'll be fine.
	 */
	SetConsoleOutputCP(CP_UTF8);

	myConsoleWindow = GetConsoleWindow();
	RECT consoleSize;
	GetWindowRect(myConsoleWindow, &consoleSize);
	MoveWindow(myConsoleWindow, consoleSize.left, consoleSize.top, 1280, 720, true);
#endif

	if (!InitializeEngine())
	{
		return;
	}

	InitializeApplication();

	std::thread gameUpdate(&Engine::Update, std::ref(Engine::GetInstance()));

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

			//if (msg.message == WM_ENTERSIZEMOVE)
			//{
			//	isPaused = true;
			//	isResizing = true;
			//}

			//if (msg.message == WM_EXITSIZEMOVE)
			//{
			//	isPaused = false;
			//	isResizing = false;

			//	RECT clientRect = {};
			//	GetClientRect(myMainWindowHandle, &clientRect);
			//	const float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
			//	const float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

			//	GraphicsEngine::Get().SetResolution(clientWidth, clientHeight);
			//	GraphicsEngine::Get().SetWindowSize(clientWidth, clientHeight);
			//}

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
	Engine::GetInstance();

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