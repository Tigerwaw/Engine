#pragma once
#include "ModelViewer.h"

#include <filesystem>
#include <future>
#include <iostream>
#include <vector>

#include <wrl.h>
using namespace Microsoft::WRL;

DEFINE_LOG_CATEGORY(LogModelViewer);

#include <d3d11.h>

#if _DEBUG
#include "imgui.h"
#endif 

#include "AssetManager.h"
#include "Asset.h"

#include "GameEngine/Engine.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"
#include "GameEngine/ImGui/ImGuiHandler.h"

#include "Graphics/GraphicsEngine/Objects/Spritesheet.h"

namespace CU = CommonUtilities;

ModelViewer::ModelViewer() = default;

void ModelViewer::Shutdown()
{
	Engine::GetInstance().Destroy();
}

bool ModelViewer::Initialize(WNDPROC aWindowProcess)
{
	Engine::GetInstance();

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
	windowClass.lpfnWndProc = aWindowProcess;
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

	MVLOG(Log, "Initializing Graphics Engine...");

	if (!GraphicsEngine::Get().Initialize(myMainWindowHandle))
	{
		MVLOG(Log, "Failed to Initialize Graphics Engine...");
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

	InitModelViewer();
	MVLOG(Log, "Ready!");
	return true;
}

void ModelViewer::InitModelViewer()
{
	Engine::GetInstance().GetInputHandler().SetControllerDeadZone(0.1f, 0.06f);
	Engine::GetInstance().GetAudioEngine().Initialize();
	Engine::GetInstance().GetAudioEngine().LoadBank("Master");
	Engine::GetInstance().GetAudioEngine().LoadBank("Master.strings");
	Engine::GetInstance().GetAudioEngine().LoadBank("Test");

	Engine::GetInstance().GetAudioEngine().AddBus(BusType::Music, "Music");
	Engine::GetInstance().GetAudioEngine().AddBus(BusType::Ambience, "Ambience");
	Engine::GetInstance().GetAudioEngine().AddBus(BusType::SFX, "SFX");

	Engine::GetInstance().GetDebugDrawer().InitializeDebugDrawer();
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/TestScene.json");

	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();
	inputHandler.RegisterBinaryAction("W", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("A", Keys::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("S", Keys::S, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("D", Keys::D, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RMB", Keys::MOUSERBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("MousePos", MouseMovement2D::MousePos);
	inputHandler.RegisterAnalog2DAction("MouseNDCPos", MouseMovement2D::MousePosNDC);
	inputHandler.RegisterAnalog2DAction("MouseDelta", MouseMovement2D::MousePosDelta);
	
	inputHandler.RegisterBinaryAction("A_Gamepad", ControllerButtons::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("B", ControllerButtons::B, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("X", ControllerButtons::X, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Y", ControllerButtons::Y, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Down", ControllerButtons::DPAD_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Left", ControllerButtons::DPAD_LEFT, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Up", ControllerButtons::DPAD_UP, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Right", ControllerButtons::DPAD_RIGHT, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LS", ControllerButtons::LEFT_STICK_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RS", ControllerButtons::RIGHT_STICK_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LB", ControllerButtons::LEFT_SHOULDER, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RB", ControllerButtons::RIGHT_SHOULDER, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Start", ControllerButtons::START, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Back", ControllerButtons::BACK, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("LeftStick", AnalogInput2D::LEFT_STICK);
	inputHandler.RegisterAnalog2DAction("RightStick", AnalogInput2D::RIGHT_STICK);
	inputHandler.RegisterAnalogAction("LeftTrigger", AnalogInput::LEFT_TRIGGER);
	inputHandler.RegisterAnalogAction("RightTrigger", AnalogInput::RIGHT_TRIGGER);

	inputHandler.RegisterBinaryAction("SharedAction", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("SharedAction", ControllerButtons::A, GenericInput::ActionType::Held);
}

int ModelViewer::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	bool isRunning = true;
	bool isPaused = false;
	//bool isResizing = false;

	while (isRunning)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

#if _DEBUG
			extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			if (ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam)) return true;
#endif

			if (msg.message == WM_QUIT)
			{
				isRunning = false;
			}

			if (msg.message == WM_ACTIVATE)
			{
				if (LOWORD(msg.wParam) == WA_INACTIVE)
				{
					isPaused = true;
				}
				else
				{
					isPaused = false;
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

		if (isPaused) continue;

		Engine::GetInstance().GetImGuiHandler().BeginFrame();
		GraphicsEngine::Get().BeginFrame();
		Engine::GetInstance().Update();
		GraphicsEngine::Get().RenderFrame();
		Engine::GetInstance().GetImGuiHandler().Render();
		GraphicsEngine::Get().EndFrame();
	}

	Shutdown();
	return 0;
}
