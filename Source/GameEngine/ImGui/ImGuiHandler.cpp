#include "Enginepch.h"

#include "ImGuiHandler.h"

#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/Audio/AudioEngine.h"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"


void ImGuiHandler::Initialize(HWND aMainWindowHandle)
{
	aMainWindowHandle;

#ifndef _RETAIL
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	if (!Engine::Get().GetIsFullscreen())
	{
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	}

	ImGui::StyleColorsDark();

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
	ImGuiIO& io = ImGui::GetIO();
	CU::Vector2f resolution = Engine::Get().GetResolution();
	io.DisplaySize = { resolution.x, resolution.y };
	io.DisplayFramebufferScale = { 1.0f, 1.0f };

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();


	ImGui::NewFrame();
#endif
}

void ImGuiHandler::Render()
{
#ifndef _RETAIL
	GraphicsEngine::Get().BeginEvent("ImGui");
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (!Engine::Get().GetIsFullscreen())
	{
		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
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