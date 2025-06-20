#include "Enginepch.h"

#include "ImGuiHandler.h"

#include "GraphicsEngine.h"
#include "Engine.h"
#include "Application/Window.h"
#include "Time/Timer.h"
#include "Input/InputHandler.h"
#include "SceneHandler/SceneHandler.h"
#include "Audio/AudioEngine.h"

#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Graphics/Model.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "ComponentSystem/Components/Graphics/Camera.h"


void ImGuiHandler::Initialize(HWND aMainWindowHandle)
{
	aMainWindowHandle;

	ImGui_ImplWin32_EnableDpiAwareness();

#ifndef _RETAIL
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigDpiScaleFonts = true;
	//io.ConfigDpiScaleViewports = true;

	ImGui::StyleColorsDark();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(aMainWindowHandle);
#endif
}

void ImGuiHandler::Destroy()
{
#ifndef _RETAIL
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void ImGuiHandler::BeginFrame()
{
#ifndef _RETAIL
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImGuiHandler::Render()
{
#ifndef _RETAIL
	if (!Engine::Get().GetApplicationWindow().GetWindowHandle()) return;

	GraphicsEngine::Get().BeginEvent("ImGui");
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	GraphicsEngine::Get().EndEvent();
#endif
}

void ImGuiHandler::Update()
{
	for (auto& func : myLambdaList)
	{
		func();
	}
}

void ImGuiHandler::AddNewFunction(std::function<void()> aFunction)
{
	myLambdaList.push_back(aFunction);
}