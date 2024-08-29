#include "Enginepch.h"
#include "Window.h"

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

bool Window::InitializeWindow(std::string aWindowTitle, CU::Vector2f aWindowSize, bool aIsFullscreen, bool aIsBorderless, bool aAllowDropFiles)
{
    LOG(LogGameEngine, Log, "Initializing window...");
    
	SIZE windowSize = { static_cast<long>(aWindowSize.x), static_cast<long>(aWindowSize.y) };
	std::wstring tempString = std::wstring(aWindowTitle.begin(), aWindowTitle.end());
	LPCWSTR windowTitle = tempString.c_str();
	constexpr LPCWSTR windowClassName = L"MainWindow";

	if (aIsFullscreen)
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

	if (aIsBorderless)
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

	DragAcceptFiles(myMainWindowHandle, static_cast<BOOL>(aAllowDropFiles));

	ShowWindow(myMainWindowHandle, SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);

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