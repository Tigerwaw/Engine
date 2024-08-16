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
#ifdef _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(aMainWindowHandle);
#endif
}

void ImGuiHandler::Destroy()
{
#ifdef _DEBUG
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void ImGuiHandler::BeginFrame()
{
#ifdef _DEBUG
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	CU::Vector2f resolution = Engine::GetInstance().GetResolution();
	io.DisplaySize = { resolution.x, resolution.y };
	io.DisplayFramebufferScale = { 1.0f, 1.0f };

	ImGui::NewFrame();
#endif
}

void ImGuiHandler::Render()
{
#ifdef _DEBUG
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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