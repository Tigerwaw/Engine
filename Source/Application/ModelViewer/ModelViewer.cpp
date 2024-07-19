#pragma once
#include "ModelViewer.h"

#include <filesystem>
#include <future>
#include <iostream>
#include <vector>

#include <wrl.h>
#include <Psapi.h>
using namespace Microsoft::WRL;

DEFINE_LOG_CATEGORY(LogModelViewer);

#include <d3d11.h>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Animation.h"

#include "AssetManager.h"
#include "Asset.h"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/SpotLight.h"

#include "GameEngine/ComponentSystem/Components/Movement/Rotator.h"
#include "GameEngine/ComponentSystem/Components/Movement/FreecamController.h"

#include "GameEngine/Engine.h"
#include "GameEngine/EngineSettings.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"

namespace CU = CommonUtilities;

ModelViewer::ModelViewer() = default;

void ModelViewer::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ModelViewer::Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle)
{
	constexpr LPCWSTR windowClassName = L"ModelViewerMainWindow";

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = aWindowProcess;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	LONG posX = (GetSystemMetrics(SM_CXSCREEN) - aWindowSize.cx) / 2;
	if (posX < 0)
		posX = 0;

	LONG posY = (GetSystemMetrics(SM_CYSCREEN) - aWindowSize.cy) / 2;
	if (posY < 0)
		posY = 0;

	// Then we use the class to create our window
	myMainWindowHandle = CreateWindow(
		windowClassName,                                // Classname
		aWindowTitle,                                   // Window Title
		WS_OVERLAPPEDWINDOW | WS_POPUP,                 // Flags
		posX,
		posY,
		aWindowSize.cx,
		aWindowSize.cy,
		nullptr, nullptr, nullptr,
		nullptr
	);

	// Setup Dear ImGui context
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(myMainWindowHandle);
	}

	{   // Graphics Init
		MVLOG(Log, "Initializing Graphics Engine...");

		if (!GraphicsEngine::Get().Initialize(myMainWindowHandle))
			return false;
	}

	MVLOG(Log, "Ready!");

	ShowWindow(myMainWindowHandle, SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);

	Engine::GetInstance();
	InitModelViewer();

	return true;
}

void ModelViewer::InitModelViewer()
{
	AssetManager::Get().Initialize(EngineSettings::GetContentRootPath());

	Engine::GetInstance().GetDebugDrawer().InitializeDebugDrawer();
	Engine::GetInstance().GetSceneHandler().CreateEmptyScene();
	InitCamera();
	InitLights();
	InitGameObjects();
}

int ModelViewer::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	myIsRunning = true;

	while (myIsRunning)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			if (ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam)) return true;

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
			}

			Engine::GetInstance().GetInputHandler().UpdateEvents(msg.message, msg.wParam, msg.lParam);
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		UpdateImgui();

		// TODO: Frame Update and Rendering goes here
		GraphicsEngine::Get().BeginFrame();
		Engine::GetInstance().GetDebugDrawer().ClearObjects();

		Engine::GetInstance().GetTimer().Update();
		Engine::GetInstance().GetSceneHandler().UpdateActiveScene();
		Engine::GetInstance().GetInputHandler().UpdateInput();

		Engine::GetInstance().GetSceneHandler().RenderActiveScene();

		Engine::GetInstance().GetDebugDrawer().DrawObjects();
		GraphicsEngine::Get().RenderFrame();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		GraphicsEngine::Get().EndFrame();
	}

	Shutdown();
	return 0;
}

void ModelViewer::InitCamera()
{
	std::shared_ptr<GameObject> camera = std::make_shared<GameObject>();
	camera->SetName("MainCamera");
	camera->AddComponent<Camera>(90.0f, 1.0f, 10000.0f, CU::Vector2<float>(1920, 1080));
	camera->AddComponent<FreecamController>(400.0f, 300.0f);
	camera->Transform.SetTranslation(0, 100.0f, -300.0f);
	Engine::GetInstance().GetSceneHandler().Instantiate(camera);
}

void ModelViewer::InitLights()
{
	std::shared_ptr<GameObject> ambientLight = std::make_shared<GameObject>();
	ambientLight->SetName("A_Light");
	ambientLight->AddComponent<AmbientLight>(AssetManager::Get().GetAsset<TextureAsset>("Textures/Cubemaps/Skansen_Cube.dds")->texture);
	Engine::GetInstance().GetSceneHandler().Instantiate(ambientLight);

	std::shared_ptr<GameObject> directionalLight = std::make_shared<GameObject>();
	directionalLight->SetName("D_Light");
	std::shared_ptr<DirectionalLight> dLight = directionalLight->AddComponent<DirectionalLight>();
	dLight->EnableShadowCasting(2048, 2048);
	dLight->SetShadowBias(0.0003f, 0.00175f);
	directionalLight->AddComponent<Camera>(-1024.0f, 1024.0f, 1024.0f, -1024.0f, 1.0f, 6000.0f);
	directionalLight->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/DirectionalLightGizmo.fbx")->mesh);
	directionalLight->Transform.SetRotation(45.0f, 45.0f, 0);
	directionalLight->Transform.SetTranslation(-500.0f, 500.0f, -500.0f);
	Engine::GetInstance().GetSceneHandler().Instantiate(directionalLight);

	std::shared_ptr<GameObject> pointLight = std::make_shared<GameObject>();
	pointLight->SetName("P_Light");
	std::shared_ptr<PointLight> pLight = pointLight->AddComponent<PointLight>(50000.0f);
	pLight->EnableShadowCasting(512, 512);
	pLight->SetShadowBias(0, 0.00006f);
	pointLight->AddComponent<Camera>(90.0f, 1.0f, 3000.0f, CU::Vector2f(512.0f, 512.0f));
	pointLight->Transform.SetTranslation(50, 200.0f, 100.0f);
	pointLight->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/PointLightGizmo.fbx")->mesh);
	Engine::GetInstance().GetSceneHandler().Instantiate(pointLight);

	std::shared_ptr<GameObject> spotLight = std::make_shared<GameObject>();
	spotLight->SetName("S_Light");
	std::shared_ptr<SpotLight> sLight = spotLight->AddComponent<SpotLight>(600.0f, 100000.0f);
	sLight->EnableShadowCasting(512, 512);
	sLight->SetShadowBias(0, 0.0001f);
	spotLight->AddComponent<Camera>(90.0f, 1.0f, 2000.0f, CU::Vector2f(512.0f, 512.0f));
	spotLight->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/SpotLightGizmo.fbx")->mesh);
	spotLight->Transform.SetRotation(45.0f, -90.0f, 0);
	spotLight->Transform.SetTranslation(-150.0f, 150.0f, 0);
	Engine::GetInstance().GetSceneHandler().Instantiate(spotLight);
}

void ModelViewer::InitGameObjects()
{
	std::shared_ptr<GameObject> plane = std::make_shared<GameObject>();
	plane->Transform = CU::Transform<float>({ 0, 0, 0 }, { 0, 0, 0 }, { 800.0f, 800.0f, 800.0f });
	plane->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("PlanePrimitive")->mesh,
							    AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(plane);
	
	
	std::shared_ptr<GameObject> cube = std::make_shared<GameObject>();
	cube->SetName("Cube");
	cube->Transform = CU::Transform<float>({ 200.0f, 50.0f, 200.0f }, { 0, 0, 0 }, { 50.0f, 50.0f, 50.0f });
	cube->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("CubePrimitive")->mesh,
							   AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(cube);

	std::shared_ptr<GameObject> ramp = std::make_shared<GameObject>();
	ramp->SetName("Ramp");
	ramp->Transform = CU::Transform<float>({ 300.0f, 50.0f, 0 }, { 0, 45.0f, 0 }, { 50.0f, 50.0f, 50.0f });
	ramp->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("RampPrimitive")->mesh,
							   AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(ramp);

	std::shared_ptr<GameObject> chest = std::make_shared<GameObject>();
	chest->SetName("Chest");
	chest->Transform = CU::Transform<float>({ -500.0f, 0, 0 }, { 0, 180.0f, 0 });
	chest->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("Models/SM_Chest.fbx")->mesh,
							    AssetManager::Get().GetAsset<MaterialAsset>("Materials/Chest.json")->material);
	chest->AddComponent<Rotator>(CU::Vector3f(0, 30.0f, 0));
	chest->AddComponent<Rotator>(CU::Vector3f(0, 30.0f, 0));
	Engine::GetInstance().GetSceneHandler().Instantiate(chest);

	std::shared_ptr<GameObject> colorChecker = std::make_shared<GameObject>();
	colorChecker->Transform = CU::Transform<float>({ 100.0f, 40.0f, 0 }, { -90.0f, 0, 0 });
	colorChecker->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/SM_Color_Checker.fbx")->mesh,
									AssetManager::Get().GetAsset<MaterialAsset>("EngineAssets/Materials/ColorChecker.json")->material);
	colorChecker->AddComponent<Rotator>(CU::Vector3f(0, 30.0f, 0));
	Engine::GetInstance().GetSceneHandler().Instantiate(colorChecker);


	std::shared_ptr<GameObject> matballOne = std::make_shared<GameObject>();
	matballOne->Transform = CU::Transform<float>({ -100.0f, 0, 0 }, { 0, -45.0f, 0 });
	std::shared_ptr<Model> matballOneModel = matballOne->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("Models/TMA_Matball.fbx")->mesh,
									AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballOne.json")->material);
	matballOneModel->SetMaterialOnSlot(1, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballTwo.json")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(matballOne);

	std::shared_ptr<GameObject> matballTwo = std::make_shared<GameObject>();
	matballTwo->Transform = CU::Transform<float>({ -100.0f, 0, 200.0f }, { 0, -45.0f, 0 });
	std::shared_ptr<Model> matballTwoModel = matballTwo->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("Models/TMA_Matball.fbx")->mesh,
									AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballTwo.json")->material);
	matballTwoModel->SetMaterialOnSlot(1, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballOne.json")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(matballTwo);

	std::shared_ptr<GameObject> tgaBro = std::make_shared<GameObject>();
	tgaBro->SetName("TgaBro");
	tgaBro->Transform = CU::Transform<float>({ -250.0f, 0, 150.0f }, { 0, 180.0f, 0 });
	tgaBro->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Models/SK_C_TGA_Bro.fbx")->mesh,
										 AssetManager::Get().GetAsset<MaterialAsset>("Materials/TgaBro.json")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(tgaBro);

	myAnimations.emplace_back(AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Brething.fbx")->animation);
	myAnimationNames.emplace_back("Idle");
	myAnimations.emplace_back(AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Locomotion/A_C_TGA_Bro_Walk.fbx")->animation);
	myAnimationNames.emplace_back("Walk");
	myAnimations.emplace_back(AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Locomotion/A_C_TGA_Bro_Run.fbx")->animation);
	myAnimationNames.emplace_back("Run");
	myAnimations.emplace_back(AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Wave.fbx")->animation);
	myAnimationNames.emplace_back("Wave");

	std::shared_ptr<AnimatedModel> tgaBroModel = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("TgaBro")->GetComponent<AnimatedModel>();
	tgaBroModel->SetAnimation(myAnimations[0], 0, "", 0, true);
	tgaBroModel->PlayAnimation();
	tgaBroModel->AddAnimationLayer("RightShoulder");
	tgaBroModel->SetAnimation(myAnimations[3], 0, "RightShoulder", 0, true);
	tgaBroModel->PlayAnimation();
}

void ModelViewer::ChangeAnimation(int aIndex)
{
	if (aIndex >= myAnimations.size() || aIndex < 0) return;

	myCurrentAnimIndex = aIndex;
	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("TgaBro")->GetComponent<AnimatedModel>()->SetAnimation(myAnimations[aIndex], 0, "", 1.0f, true);
}

void ModelViewer::UpdateImgui()
{
	// Various
	{
		ImGui::SetNextWindowPos({ 20.0f, 20.0f });
		ImGui::SetNextWindowContentSize({ 300.0f, 200.0f });
		ImGui::Begin("Modelviewer");

		ImGui::Checkbox("Use Viewculling", &GraphicsEngine::Get().UseViewCulling);
		ImGui::Checkbox("Draw Gizmos", &GraphicsEngine::Get().DrawGizmos);
		ImGui::Checkbox("Draw Bounding Boxes", &GraphicsEngine::Get().DrawBoundingBoxes);
		ImGui::Checkbox("Draw Camera Frustums", &GraphicsEngine::Get().DrawCameraFrustums);

		// Rendering
		{
			ImGui::Text("Rendering Mode");
			if (ImGui::BeginCombo("##RenderModeDropdown", GraphicsEngine::Get().DebugModeNames[static_cast<int>(GraphicsEngine::Get().GetCurrentDebugMode())].c_str()))
			{
				if (ImGui::Selectable("None")) GraphicsEngine::Get().SetDebugMode(DebugMode::None);
				if (ImGui::Selectable("Unlit")) GraphicsEngine::Get().SetDebugMode(DebugMode::Unlit);
				if (ImGui::Selectable("Wireframe")) GraphicsEngine::Get().SetDebugMode(DebugMode::Wireframe);
				if (ImGui::Selectable("DebugVertexNormals")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugVertexNormals);
				if (ImGui::Selectable("DebugPixelNormals")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugPixelNormals);
				if (ImGui::Selectable("DebugTextureNormals")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugTextureNormals);
				if (ImGui::Selectable("DebugUVs")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugUVs);
				ImGui::EndCombo();
			}
		}

		ImGui::Separator();

		// Animation
		{
			ImGui::Text("Animations");
			if (ImGui::BeginCombo("##AnimationDropdown", myAnimationNames[myCurrentAnimIndex].c_str()))
			{
				if (ImGui::Selectable("Idle")) ChangeAnimation(0);
				if (ImGui::Selectable("Walk")) ChangeAnimation(1);
				if (ImGui::Selectable("Run")) ChangeAnimation(2);
				if (ImGui::Selectable("Wave")) ChangeAnimation(3);
				ImGui::EndCombo();
			}
		}

		ImGui::End();
	}

	// Lighting
	{
		ImGui::SetNextWindowPos({ 20.0f, 300.0f });
		ImGui::SetNextWindowContentSize({ 300.0f, 600.0f });
		ImGui::Begin("Lighting Settings");
		{
			std::shared_ptr<GameObject> ambientLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("A_Light");
			bool active = ambientLight->GetActive();
			if (ImGui::Checkbox("##AmbLightCheck", &active)) ambientLight->SetActive(active);
			ImGui::SameLine();
			if (ImGui::CollapsingHeader("Ambient Light"))
			{
				std::shared_ptr<AmbientLight> aLight = ambientLight->GetComponent<AmbientLight>();
				float intensity = aLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##AmbientIntensity", &intensity, 0, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) aLight->SetIntensity(intensity);
				ImGui::Spacing();
				float color[3] = { aLight->GetColor().x, aLight->GetColor().y, aLight->GetColor().z };
				ImGui::Text("Color");
				if (ImGui::ColorPicker3("##AmbientColor", color)) aLight->SetColor({ color[0], color[1], color[2] });
				ImGui::Separator();
			}
		}
		ImGui::Spacing();
		{
			std::shared_ptr<GameObject> directionalLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("D_Light");
			bool active = directionalLight->GetActive();
			if (ImGui::Checkbox("##DLightCheck", &active)) directionalLight->SetActive(active);
			ImGui::SameLine();
			if (ImGui::CollapsingHeader("Directional Light"))
			{
				std::shared_ptr<DirectionalLight> dLight = directionalLight->GetComponent<DirectionalLight>();
				float intensity = dLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##DirectionalIntensity", &intensity, 0, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) dLight->SetIntensity(intensity);
				ImGui::Spacing();
				float minBias = dLight->GetMinShadowBias();
				float maxBias = dLight->GetMaxShadowBias();
				ImGui::Text("Shadow Min Bias");
				if (ImGui::SliderFloat("##DirectionalMinBias", &minBias, 0, 0.001f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Shadow Max Bias");
				if (ImGui::SliderFloat("##DirectionalMaxBias", &maxBias, 0, 0.003f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
				ImGui::Spacing();
				float color[3] = { dLight->GetColor().x, dLight->GetColor().y, dLight->GetColor().z };
				ImGui::Text("Color");
				if (ImGui::ColorPicker3("##DirectionalColor", color)) dLight->SetColor({ color[0], color[1], color[2] });
			}
		}

		ImGui::Spacing();
		{
			std::shared_ptr<GameObject> pointLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("P_Light");
			bool active = pointLight->GetActive();
			if (ImGui::Checkbox("##PLightCheck", &active))  pointLight->SetActive(active);
			ImGui::SameLine();
			if (ImGui::CollapsingHeader("Pointlight"))
			{
				std::shared_ptr<PointLight> pLight = pointLight->GetComponent<PointLight>();
				float value = pLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##PointIntensity", &value, 0, 500000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) pLight->SetIntensity(value);

				ImGui::Spacing();

				float minBias = pLight->GetMinShadowBias();
				float maxBias = pLight->GetMaxShadowBias();
				ImGui::Text("Shadow Min Bias");
				if (ImGui::SliderFloat("##PointMinBias", &minBias, 0, 0.001f, "%.5f")) pLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Shadow Max Bias");
				if (ImGui::SliderFloat("##PointMaxBias", &maxBias, 0, 0.001f, "%.5f")) pLight->SetShadowBias(minBias, maxBias);
				ImGui::Spacing();

				float color[3] = { pLight->GetColor().x, pLight->GetColor().y, pLight->GetColor().z };
				ImGui::Text("Color");
				if (ImGui::ColorPicker3("##PointColor", color)) pLight->SetColor({ color[0], color[1], color[2] });

				ImGui::Spacing();

				float pos[3] = { pLight->GetPosition().x, pLight->GetPosition().y, pLight->GetPosition().z };
				ImGui::Text("Position");
				if (ImGui::DragFloat3("##PLightPos", pos)) pLight->GetParent()->Transform.SetTranslation(pos[0], pos[1], pos[2]);
			}
		}

		ImGui::Spacing();
		{
			std::shared_ptr<GameObject> spotLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("S_Light");
			bool active = spotLight->GetActive();
			if (ImGui::Checkbox("##SLightCheck", &active)) spotLight->SetActive(active);
			ImGui::SameLine();
			if (ImGui::CollapsingHeader("Spotlight"))
			{
				std::shared_ptr<SpotLight> sLight = spotLight->GetComponent<SpotLight>();
				float intensity = sLight->GetIntensity();
				ImGui::Text("Intensity");
				if (ImGui::SliderFloat("##SpotIntensity", &intensity, 0, 1000000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) sLight->SetIntensity(intensity);
				ImGui::Spacing();
				float angle = sLight->GetConeAngleDegrees();
				ImGui::Text("Focus");
				if (ImGui::SliderFloat("##SpotConeAngle", &angle, 600.0f, 50000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) sLight->SetConeAngle(angle);
				ImGui::Spacing();

				float minBias = sLight->GetMinShadowBias();
				float maxBias = sLight->GetMaxShadowBias();
				ImGui::Text("Shadow Min Bias");
				if (ImGui::SliderFloat("##SpotMinBias", &minBias, 0, 0.001f, "%.5f")) sLight->SetShadowBias(minBias, maxBias);
				ImGui::Text("Shadow Max Bias");
				if (ImGui::SliderFloat("##SpotMaxBias", &maxBias, 0, 0.001f, "%.5f")) sLight->SetShadowBias(minBias, maxBias);

				ImGui::Spacing();
				float color[3] = { sLight->GetColor().x, sLight->GetColor().y, sLight->GetColor().z };
				ImGui::Text("Color");
				if (ImGui::ColorPicker3("##SpotColor", color)) sLight->SetColor({ color[0], color[1], color[2] });
				
				ImGui::Spacing();

				float pos[3] = { sLight->GetPosition().x, sLight->GetPosition().y, sLight->GetPosition().z };
				ImGui::Text("Position");
				if (ImGui::DragFloat3("##SLightPos", pos)) sLight->GetParent()->Transform.SetTranslation(pos[0], pos[1], pos[2]);
				CU::Vector3f rotation = sLight->GetParent()->Transform.GetRotation();
				float rot[3] = { rotation.x, rotation.y, rotation.z};
				ImGui::Text("Rotation");
				if (ImGui::DragFloat3("##SLightRot", rot)) sLight->GetParent()->Transform.SetRotation(rot[0], rot[1], rot[2]);
			}
		}
		
		ImGui::End();
	}

	// Performance Info
	{
		ImGui::SetNextWindowPos({ 350.0f, 20.0f });
		ImGui::SetNextWindowContentSize({ 300.0f, 200.0f });
		ImGui::Begin("Performance Info");
		ImGui::BeginTable("PerformanceTable", 2, 0, { 350.0f, 0 });
		ImGuiStyle& style = ImGui::GetStyle();
		style.CellPadding = { 20.0f, 4.0f };

		// FPS
		{
			CU::Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
			int fps = Engine::GetInstance().GetTimer().GetFPS();
			if (fps < 60) color = { 1.0f, 1.0f, 0.0f, 1.0f };
			if (fps < 30) color = { 1.0f, 0.0f, 0.0f, 1.0f };
			
			ImGui::TableNextColumn();
			ImGui::Text("FPS:");
			ImGui::TableNextColumn();
			ImGui::TextColored({ color.x, color.y, color.z, color.w }, std::to_string(fps).c_str());
			ImGui::TableNextColumn();

			ImGui::Text("Frametime (ms):");
			ImGui::TableNextColumn();
			ImGui::Text("%.2f", Engine::GetInstance().GetTimer().GetFrameTimeMS());
			ImGui::TableNextColumn();
		}

		// Draw calls
		{
			ImGui::Text("Drawcalls:");
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(GraphicsEngine::Get().GetDrawcallAmount()).c_str());
			ImGui::TableNextColumn();
		}

		// Scene objects
		{
			ImGui::Text("Scene Objects:");
			ImGui::TableNextColumn();
			ImGui::Text(std::string(std::to_string(Engine::GetInstance().GetSceneHandler().GetObjectAmount())).c_str());

			ImGui::TableNextColumn();

			ImGui::Text("Active Scene Objects:");
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(Engine::GetInstance().GetSceneHandler().GetActiveObjectAmount()).c_str());
			ImGui::TableNextColumn();
		}

		ImGui::Spacing();

		// Memory Usage
		{
			Engine::GetInstance().TimeSinceLastMemoryCheck += Engine::GetInstance().GetTimer().GetDeltaTime();
			if (Engine::GetInstance().TimeSinceLastMemoryCheck > Engine::GetInstance().MemoryCheckTimeInterval)
			{
				Engine::GetInstance().TimeSinceLastMemoryCheck = 0;

				HANDLE hProcess = {};
				PROCESS_MEMORY_COUNTERS pmc;
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());

				if (hProcess != NULL)
				{
					if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
					{
						int newRamUsage = static_cast<int>(pmc.WorkingSetSize / (1024.0 * 1024.0));
						Engine::GetInstance().RamUsageChange = newRamUsage - Engine::GetInstance().RamUsage;
						Engine::GetInstance().RamUsage = newRamUsage;
					}

					CloseHandle(hProcess);
				}
			}

			CU::Vector4f color = { 1.0f, 0.0f, 0.0f, 1.0f };
			if (Engine::GetInstance().RamUsageChange <= 0) color = { 0.0f, 1.0f, 0.0f, 1.0f };

			ImGui::Text("RAM used:");
			ImGui::TableNextColumn();
			ImGui::Text(std::string(std::to_string(Engine::GetInstance().RamUsage) + " Mb").c_str());

			ImGui::TableNextColumn();

			ImGui::Text("RAM usage fluctuation:");
			ImGui::TableNextColumn();
			ImGui::TextColored({ color.x, color.y, color.z, color.w }, std::string(std::to_string(Engine::GetInstance().RamUsageChange) + " Mb").c_str());
			ImGui::TableNextColumn();
		}

		ImGui::EndTable();
		ImGui::End();
	}
}
