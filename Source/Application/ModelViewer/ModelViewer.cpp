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

#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Animation.h"

#include "AssetManager.h"
#include "Asset.h"

#include "GameEngine/Engine.h"
#include "GameEngine/EngineSettings.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/Input/InputHandler.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Audio/AudioEngine.h"
#include "GameEngine/Audio/AudioInstance.h"
#include "GameEngine/ImGui/ImGuiHandler.h"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/AudioSource.h"
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
#include "GameEngine/ComponentSystem/Components/Movement/ObjectController.h"

namespace CU = CommonUtilities;

ModelViewer::ModelViewer() = default;

void ModelViewer::Shutdown()
{
	Engine::GetInstance().Destroy();
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

	MVLOG(Log, "Initializing Graphics Engine...");

	if (!GraphicsEngine::Get().Initialize(myMainWindowHandle))
	{
		MVLOG(Log, "Failed to Initialize Graphics Engine...");
		return false;
	}

	MVLOG(Log, "Initializing Game Engine...");
	Engine::GetInstance();
	AssetManager::Get().Initialize(EngineSettings::GetContentRootPath());

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
	Engine::GetInstance().GetSceneHandler().CreateEmptyScene();
	InitCamera();
	InitLights();
	InitGameObjects();

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

	myIsRunning = true;

	while (myIsRunning)
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
				myIsRunning = false;
			}

			Engine::GetInstance().GetInputHandler().UpdateEvents(msg.message, msg.wParam, msg.lParam);
		}

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

void ModelViewer::InitCamera()
{
	std::shared_ptr<GameObject> camera = std::make_shared<GameObject>();
	camera->AddComponent<Transform>(CU::Vector3f(0, 600.0f, -600.0f), CU::Vector3f(45.0f, 0, 0));
	camera->SetName("MainCamera");
	camera->AddComponent<Camera>(90.0f, 1.0f, 10000.0f, CU::Vector2<float>(1920, 1080));
	camera->AddComponent<FreecamController>(400.0f, 300.0f);
	Engine::GetInstance().GetSceneHandler().Instantiate(camera);

	Engine::GetInstance().GetAudioEngine().SetListener(camera);
}

void ModelViewer::InitLights()
{
	std::shared_ptr<GameObject> ambientLight = std::make_shared<GameObject>();
	ambientLight->SetName("A_Light");
	ambientLight->AddComponent<AmbientLight>(AssetManager::Get().GetAsset<TextureAsset>("Textures/Cubemaps/Skansen_Cube.dds")->texture);
	Engine::GetInstance().GetSceneHandler().Instantiate(ambientLight);

	std::shared_ptr<GameObject> directionalLight = std::make_shared<GameObject>();
	directionalLight->AddComponent<Transform>();
	directionalLight->SetName("D_Light");
	std::shared_ptr<DirectionalLight> dLight = directionalLight->AddComponent<DirectionalLight>();
	dLight->EnableShadowCasting(2048, 2048);
	dLight->SetShadowBias(0.0003f, 0.00175f);
	directionalLight->AddComponent<Camera>(-1024.0f, 1024.0f, 1024.0f, -1024.0f, 1.0f, 6000.0f);
	directionalLight->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/DirectionalLightGizmo.fbx")->mesh);
	directionalLight->GetComponent<Transform>()->SetRotation(45.0f, 45.0f, 0);
	directionalLight->GetComponent<Transform>()->SetTranslation(-500.0f, 500.0f, -500.0f);
	Engine::GetInstance().GetSceneHandler().Instantiate(directionalLight);

	std::shared_ptr<GameObject> pointLight = std::make_shared<GameObject>();
	pointLight->AddComponent<Transform>();
	pointLight->SetName("P_Light");
	std::shared_ptr<PointLight> pLight = pointLight->AddComponent<PointLight>(50000.0f);
	pLight->EnableShadowCasting(512, 512);
	pLight->SetShadowBias(0, 0.00006f);
	pointLight->AddComponent<Camera>(90.0f, 1.0f, 3000.0f, CU::Vector2f(512.0f, 512.0f));
	pointLight->GetComponent<Transform>()->SetTranslation(50, 200.0f, 100.0f);
	pointLight->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/PointLightGizmo.fbx")->mesh);
	Engine::GetInstance().GetSceneHandler().Instantiate(pointLight);

	std::shared_ptr<GameObject> spotLight = std::make_shared<GameObject>();
	spotLight->AddComponent<Transform>();
	spotLight->SetName("S_Light");
	std::shared_ptr<SpotLight> sLight = spotLight->AddComponent<SpotLight>(600.0f, 100000.0f);
	sLight->EnableShadowCasting(512, 512);
	sLight->SetShadowBias(0, 0.0001f);
	spotLight->AddComponent<Camera>(90.0f, 1.0f, 2000.0f, CU::Vector2f(512.0f, 512.0f));
	spotLight->AddComponent<DebugModel>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/SpotLightGizmo.fbx")->mesh);
	spotLight->GetComponent<Transform>()->SetRotation(45.0f, -90.0f, 0);
	spotLight->GetComponent<Transform>()->SetTranslation(-150.0f, 150.0f, 0);
	Engine::GetInstance().GetSceneHandler().Instantiate(spotLight);
}

void ModelViewer::InitGameObjects()
{
	std::shared_ptr<GameObject> plane = std::make_shared<GameObject>();
	plane->AddComponent<Transform>(CU::Vector3f(), CU::Vector3f(), CU::Vector3f(800.0f, 800.0f, 800.0f));
	plane->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("PlanePrimitive")->mesh,
							    AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(plane);
	
	
	std::shared_ptr<GameObject> cube = std::make_shared<GameObject>();
	cube->SetName("Cube");
	cube->AddComponent<Transform>(CU::Vector3f(200.0f, 50.0f, 200.0f), CU::Vector3f(), CU::Vector3f(50.0f, 50.0f, 50.0f));
	cube->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("CubePrimitive")->mesh,
							   AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(cube);

	std::shared_ptr<GameObject> ramp = std::make_shared<GameObject>();
	ramp->SetName("Ramp");
	ramp->AddComponent<Transform>(CU::Vector3f(300.0f, 50.0f, 0), CU::Vector3f(0, 45.0f, 0), CU::Vector3f(50.0f, 50.0f, 50.0f));
	ramp->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("RampPrimitive")->mesh,
							   AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(ramp);

	std::shared_ptr<GameObject> chest = std::make_shared<GameObject>();
	chest->SetName("Chest");
	chest->AddComponent<Transform>(CU::Vector3f(-500.0f, 0, 0), CU::Vector3f(0, 180.0f, 0));
	chest->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("Models/SM_Chest.fbx")->mesh,
							    AssetManager::Get().GetAsset<MaterialAsset>("Materials/Chest.json")->material);
	chest->AddComponent<Rotator>(CU::Vector3f(0, 30.0f, 0));
	chest->AddComponent<Rotator>(CU::Vector3f(0, 30.0f, 0));
	Engine::GetInstance().GetSceneHandler().Instantiate(chest);

	std::shared_ptr<GameObject> colorChecker = std::make_shared<GameObject>();
	colorChecker->SetName("ColorChecker");
	colorChecker->AddComponent<Transform>(CU::Vector3f(100.0f, 40.0f, 0), CU::Vector3f(-90.0f, 0, 0));
	colorChecker->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("EngineAssets/Models/SM_Color_Checker.fbx")->mesh,
									AssetManager::Get().GetAsset<MaterialAsset>("EngineAssets/Materials/ColorChecker.json")->material);
	colorChecker->AddComponent<Rotator>(CU::Vector3f(0, 30.0f, 0));
	Engine::GetInstance().GetSceneHandler().Instantiate(colorChecker);


	std::shared_ptr<GameObject> matballOne = std::make_shared<GameObject>();
	matballOne->SetName("MatballOne");
	matballOne->AddComponent<Transform>(CU::Vector3f(-100.0f, 0, 0), CU::Vector3f(0, -45.0f, 0));
	matballOne->AddComponent<Rotator>(CU::Vector3f(0, 30.0f, 0));
	std::shared_ptr<Model> matballOneModel = matballOne->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("Models/TMA_Matball.fbx")->mesh,
									AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballOne.json")->material);
	matballOneModel->SetMaterialOnSlot(1, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballTwo.json")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(matballOne);

	std::shared_ptr<GameObject> matballTwo = std::make_shared<GameObject>();
	matballTwo->SetName("MatballTwo");
	matballTwo->AddComponent<Transform>(CU::Vector3f(-100.0f, 0, 200.0f), CU::Vector3f(0, -45.0f, 0));
	std::shared_ptr<Model> matballTwoModel = matballTwo->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("Models/TMA_Matball.fbx")->mesh,
									AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballTwo.json")->material);
	matballTwoModel->SetMaterialOnSlot(1, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MatballOne.json")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(matballTwo);
	std::shared_ptr<AudioSource> matballTwoAudio = matballTwo->AddComponent<AudioSource>();
	matballTwoAudio->AddAudioInstance("TestMusic", false, AudioSource::SourceType::Following);
	matballTwoAudio->Play("TestMusic");

	matballOne->GetComponent<Transform>()->AddChild(matballTwo->GetComponent<Transform>().get());

	std::shared_ptr<GameObject> tgaBro = std::make_shared<GameObject>();
	tgaBro->SetName("TgaBro");
	tgaBro->AddComponent<Transform>(CU::Vector3f(-250.0f, 0, 150.0f), CU::Vector3f(0, 180.0f, 0));
	tgaBro->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Models/SK_C_TGA_Bro.fbx")->mesh,
										 AssetManager::Get().GetAsset<MaterialAsset>("Materials/TgaBro.json")->material);
	Engine::GetInstance().GetSceneHandler().Instantiate(tgaBro);
	std::shared_ptr<AudioSource> tgaBroAudio = tgaBro->AddComponent<AudioSource>();
	tgaBroAudio->AddAudioInstance("TgaBroFootsteps", true, AudioSource::SourceType::AtLocation);
	tgaBroAudio->AddAudioPlayOnEvent("TgaBroFootsteps", GameObjectEventType::Footstep);

	std::shared_ptr<AnimatedModel> tgaBroModel = tgaBro->GetComponent<AnimatedModel>();
	tgaBroModel->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Brething.fbx")->animation, "", true);
	tgaBroModel->AddAnimationToLayer("Walk", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Locomotion/A_C_TGA_Bro_Walk.fbx")->animation, "", true);
	tgaBroModel->AddAnimationToLayer("Run", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Locomotion/A_C_TGA_Bro_Run.fbx")->animation, "", true);
	tgaBroModel->AddAnimationToLayer("Wave", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Wave.fbx")->animation, "", true);
	tgaBroModel->AddAnimationLayer("RightShoulder");
	tgaBroModel->AddAnimationToLayer("Wave", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Wave.fbx")->animation, "RightShoulder", true);
	tgaBroModel->PlayAnimation();
	tgaBroModel->AddAnimationEvent("Walk", 1, GameObjectEventType::Footstep, "");
	tgaBroModel->AddAnimationEvent("Walk", 17, GameObjectEventType::Footstep, "");
	tgaBroModel->AddAnimationEvent("Run", 4, GameObjectEventType::Footstep, "");
	tgaBroModel->AddAnimationEvent("Run", 13, GameObjectEventType::Footstep, "");
}
