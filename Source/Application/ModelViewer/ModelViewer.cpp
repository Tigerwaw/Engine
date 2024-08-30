#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"

class ModelViewer : public Application
{
public:
	ModelViewer() {}
    ~ModelViewer() {}

    void InitializeApplication() override;
	void UpdateApplication() override;
private:
	void ResetScene();
	void ResetGameObject(std::shared_ptr<GameObject> aGO);
	void SetModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetAnimatedModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetAnimation(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetMaterial(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetTexture(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);

	std::vector<std::string> myLogs;
	std::vector<std::string> myLoadedAssets;

	ImFont* newFont;
	unsigned currentDebugMode = 0;
};

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new ModelViewer();
}

void ModelViewer::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_ModelViewerScene.json");
	std::shared_ptr<GameObject> newGO = std::make_shared<GameObject>();
	newGO->SetName("Model");
	newGO->AddComponent<Transform>(CU::Vector3f(0, 0, 0), CU::Vector3f(0, -180.0f, 0));
	Engine::GetInstance().GetSceneHandler().Instantiate(newGO);

	std::filesystem::path fontPath = AssetManager::Get().GetContentRoot() / "Fonts/Roboto-Regular.ttf";
	newFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16.0f);
	ImGui::GetIO().Fonts->Build();

	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();
	inputHandler.RegisterBinaryAction("W", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("A", Keys::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("S", Keys::S, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("D", Keys::D, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("F6", Keys::F6, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RMB", Keys::MOUSERBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("MousePos", MouseMovement2D::MousePos);
	inputHandler.RegisterAnalog2DAction("MouseNDCPos", MouseMovement2D::MousePosNDC);
	inputHandler.RegisterAnalog2DAction("MouseDelta", MouseMovement2D::MousePosDelta);

	Engine::GetInstance().GetWindowsEventHandler().AddEvent(WindowsEventHandler::WindowsEvent::DropFiles, [this](MSG aMSG)
		{
			TCHAR name[MAX_PATH];

			HDROP hDrop = (HDROP)aMSG.wParam;

			unsigned count = DragQueryFileW(hDrop, (unsigned)0xFFFFFFFF, name, MAX_PATH);

			for (unsigned i = 0; i < count; i++)
			{
				DragQueryFile(hDrop, i, name, MAX_PATH);
				std::filesystem::path filePath = name;

				if (AssetManager::Get().RegisterAsset(filePath))
				{
					myLoadedAssets.emplace_back(filePath.string());
				}
				std::filesystem::path assetPath = AssetManager::Get().MakeRelative(filePath);
				if (assetPath == "")
				{
					LOG(LogApplication, Error, "Couldn't find asset path!");
					myLogs.emplace_back("[ERROR] Couldn't find asset path!");
					return;
				}

				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				if (!go)
				{
					LOG(LogApplication, Error, "Couldn't find game object");
					myLogs.emplace_back("[ERROR] Couldn't find game object!");
				}

				const std::string assetName = assetPath.filename().stem().string();
				if (assetName.starts_with("SM"))
				{
					SetModel(go, assetPath);
				}
				else if (assetName.starts_with("SK"))
				{
					SetAnimatedModel(go, assetPath);
				}
				else if (assetName.starts_with("A"))
				{
					SetAnimation(go, assetPath);
				}
				else if (assetName.starts_with("MAT"))
				{
					SetMaterial(go, assetPath);
				}
				else if (assetName.starts_with("T"))
				{
					SetTexture(go, assetPath);
				}
				else
				{
					LOG(LogApplication, Error, "Can't load filetype {}", assetPath.extension().string().c_str());
					myLogs.emplace_back("[ERROR] Can't load filetype" + assetPath.extension().string() + "!");
				}
			}

			DragFinish(hDrop);
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this]()
		{
#ifdef _DEBUG
			CU::Vector2f size(250.0f, 200.0f);
			float offset = 10.0f;
			CU::Vector2f windowPos = Engine::GetInstance().GetApplicationWindow().GetTopLeft();
			windowPos.x -= size.x + offset;

			ImGui::SetNextWindowPos({ windowPos.x, windowPos.y });
			ImGui::SetNextWindowContentSize({ size.x, size.y });
			ImGui::PushFont(newFont);

			bool open = true;
			ImGui::Begin("Modelviewer", &open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			// Rendering
			{
				ImGui::Text("Rendering Mode");
				if (ImGui::BeginCombo("##RenderModeDropdown", GraphicsEngine::Get().DebugModeNames[static_cast<int>(GraphicsEngine::Get().GetCurrentDebugMode())].c_str()))
				{
					if (ImGui::Selectable("None")) GraphicsEngine::Get().SetDebugMode(DebugMode::None);
					if (ImGui::Selectable("Albedo")) GraphicsEngine::Get().SetDebugMode(DebugMode::Unlit);
					if (ImGui::Selectable("Ambient Occlusion")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugAO);
					if (ImGui::Selectable("Roughness")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugRoughness);
					if (ImGui::Selectable("Metallic")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugMetallic);
					if (ImGui::Selectable("Wireframe")) GraphicsEngine::Get().SetDebugMode(DebugMode::Wireframe);
					if (ImGui::Selectable("Vertex Normals")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugVertexNormals);
					if (ImGui::Selectable("Vertex Tangents")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugVertexTangents);
					if (ImGui::Selectable("Vertex Binormals")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugVertexBinormals);
					if (ImGui::Selectable("Pixel Normals")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugPixelNormals);
					if (ImGui::Selectable("Texture Normals")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugTextureNormals);
					if (ImGui::Selectable("UV0")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugUVs);
					if (ImGui::Selectable("Vertex Color 0")) GraphicsEngine::Get().SetDebugMode(DebugMode::DebugVertexColor);
					ImGui::EndCombo();
				}
			}

			ImGui::Separator();

			if (ImGui::Button("Reset Scene"))
			{
				ResetScene();
			};

			if (ImGui::Button("Toggle Floorplane"))
			{
				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Plane");
				go->SetActive(!go->GetActive());
			};

			if (ImGui::Button("Add Cube Primitive"))
			{
				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				std::filesystem::path prim = "CubePrimitive";
				SetModel(go, prim);
				std::shared_ptr<Transform> transform = go->GetComponent<Transform>();
				transform->SetScale(50.0f, 50.0f, 50.0f);
				transform->SetTranslation(0, 100.0f, 0);
			};

			if (ImGui::Button("Add Sphere Primitive"))
			{
				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				std::filesystem::path prim = "EngineAssets/Models/SM_Sphere.fbx";
				SetModel(go, prim);
				std::shared_ptr<Transform> transform = go->GetComponent<Transform>();
				transform->SetTranslation(0, 100.0f, 0);
				transform->SetScale(50.0f, 50.0f, 50.0f);
			};

			if (ImGui::Button("Add Plane Primitive"))
			{
				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				std::filesystem::path prim = "PlanePrimitive";
				SetModel(go, prim);
				std::shared_ptr<Transform> transform = go->GetComponent<Transform>();
				transform->SetRotation(90.0f, -180.0f, 0);
				transform->SetScale(50.0f, 50.0f, 50.0f);
				transform->SetTranslation(0, 100.0f, 0);
			}

			// Animation
			//{
			//	ImGui::Text("Animations");
			//	std::shared_ptr<AnimatedModel> animModel = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model")->GetComponent<AnimatedModel>();
			//	if (animModel)
			//	{
			//		if (ImGui::BeginCombo("##AnimationDropdown", animModel->GetCurrentAnimationNameOnLayer(0).c_str()))
			//		{
			//			if (ImGui::Selectable("Idle")) animModel->SetCurrentAnimationOnLayer("Idle", "", 0.5f);
			//			if (ImGui::Selectable("Walk")) animModel->SetCurrentAnimationOnLayer("Walk", "", 0.5f);;
			//			if (ImGui::Selectable("Run")) animModel->SetCurrentAnimationOnLayer("Run", "", 0.5f);;
			//			if (ImGui::Selectable("Wave")) animModel->SetCurrentAnimationOnLayer("Wave", "", 0.5f);;
			//			ImGui::EndCombo();
			//		}
			//	}
			//}

			ImGui::End();
			ImGui::PopFont();
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this]()
		{
#ifdef _DEBUG
			CU::Vector2f size(250.0f, 500.0f);
			float offset = 10.0f;
			CU::Vector2f windowPos = Engine::GetInstance().GetApplicationWindow().GetTopLeft();
			windowPos.x -= size.x + offset;
			windowPos.y += 250.0f;

			ImGui::SetNextWindowPos({ windowPos.x, windowPos.y });
			ImGui::SetNextWindowContentSize({ size.x, size.y });
			ImGui::PushFont(newFont);
			bool open = true;
			ImGui::Begin("Lighting Settings", &open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
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
					float color[3] = { dLight->GetColor().x, dLight->GetColor().y, dLight->GetColor().z };
					ImGui::Text("Color");
					if (ImGui::ColorPicker3("##DirectionalColor", color)) dLight->SetColor({ color[0], color[1], color[2] });

					std::shared_ptr<GameObject> dLightParent = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("D_Light_Parent");
					std::shared_ptr<Transform> dlptransform = dLightParent->GetComponent<Transform>();
					CU::Vector3f rotation = dlptransform->GetRotation();
					float rot[3] = { rotation.x, rotation.y, rotation.z };
					ImGui::Text("Rotation");
					if (ImGui::DragFloat3("##DLightRot", rot)) dlptransform->SetRotation(rot[0], rot[1], rot[2]);
					//ImGui::Spacing();
					//if (ImGui::CollapsingHeader("Advanced Settings"))
					//{
					//	float minBias = dLight->GetMinShadowBias();
					//	float maxBias = dLight->GetMaxShadowBias();
					//	float lightSize = dLight->GetLightSize();
					//	ImGui::Text("Shadow Min Bias");
					//	if (ImGui::SliderFloat("##DirectionalMinBias", &minBias, 0, 0.001f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
					//	ImGui::Text("Shadow Max Bias");
					//	if (ImGui::SliderFloat("##DirectionalMaxBias", &maxBias, 0, 0.1f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
					//	ImGui::Text("Light Size");
					//	if (ImGui::SliderFloat("##DirectionalSize", &lightSize, 0, 2000.0f)) dLight->SetLightSize(lightSize);
					//}
				}
			}

			ImGui::End();
			ImGui::PopFont();
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this]()
		{
#ifdef _DEBUG
			CU::Vector2f size(300.0f, 200.0f);
			float offset = 10.0f;
			CU::Vector2f windowPos = Engine::GetInstance().GetApplicationWindow().GetTopRight();
			windowPos.x += offset;

			ImGui::SetNextWindowPos({ windowPos.x, windowPos.y });
			ImGui::SetNextWindowContentSize({ size.x, size.y });
			ImGui::PushFont(newFont);
			bool open = true;
			ImGui::Begin("Log", &open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			{
				if (myLogs.size() > 0)
				{
					for (size_t i = myLogs.size() - 1; i > 0; i--)
					{
						ImGui::Text(myLogs[i].c_str());
					}
				}
			}
			ImGui::End();
			ImGui::PopFont();
#endif
		});
}

void ModelViewer::UpdateApplication()
{
	if (Engine::GetInstance().GetInputHandler().GetBinaryAction("F6"))
	{
		currentDebugMode += 1;
		if (currentDebugMode >= static_cast<unsigned>(DebugMode::COUNT))
		{
			currentDebugMode = 0;
		}

		GraphicsEngine::Get().SetDebugMode(static_cast<DebugMode>(currentDebugMode));
	}
}

void ModelViewer::ResetScene()
{
	std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
	ResetGameObject(go);

	std::shared_ptr<GameObject> dLightParent = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("D_Light_Parent");
	std::shared_ptr<Transform> dLightTransform = dLightParent->GetComponent<Transform>();
	dLightTransform->SetRotation(0, 0, 0);

	myLogs.clear();
	myLogs.emplace_back("[LOG] Reset Scene");
}

void ModelViewer::ResetGameObject(std::shared_ptr<GameObject> aGO)
{
	std::shared_ptr<Transform> transform = aGO->GetComponent<Transform>();
	transform->SetRotation(0, -180.0f, 0);
	transform->SetScale(1.0f, 1.0f, 1.0f);

	if (aGO->GetComponent<AnimatedModel>())
	{
		aGO->RemoveComponent<AnimatedModel>();
	}

	if (aGO->GetComponent<Model>())
	{
		aGO->RemoveComponent<Model>();
	}
}

void ModelViewer::SetModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	ResetGameObject(aGO);
	aGO->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>(aAssetPath)->mesh, AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	myLogs.emplace_back("[LOG] Model set to " + aAssetPath.filename().stem().string());
}

void ModelViewer::SetAnimatedModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	ResetGameObject(aGO);
	aGO->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>(aAssetPath)->mesh, AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
	aGO->GetComponent<AnimatedModel>()->StopAnimation();
	myLogs.emplace_back("[LOG] Model set to " + aAssetPath.filename().stem().string());
}

void ModelViewer::SetAnimation(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	std::shared_ptr<AnimatedModel> animModel = aGO->GetComponent<AnimatedModel>();
	if (!animModel)
	{
		LOG(LogApplication, Warning, "The currently loaded model does not support animations!");
		myLogs.emplace_back("[ERROR] The currently loaded model does not support animations!");
		return;
	}

	std::string animName = aAssetPath.filename().stem().string();
	animModel->AddAnimationToLayer(animName, AssetManager::Get().GetAsset<AnimationAsset>(aAssetPath)->animation, "", true);
	animModel->SetCurrentAnimationOnLayer(animName, "", 0, 0);
	animModel->PlayAnimation();
	myLogs.emplace_back("[LOG] Playing animation " + aAssetPath.filename().stem().string());
}

void ModelViewer::SetMaterial(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	std::shared_ptr<Model> model = aGO->GetComponent<Model>();
	std::shared_ptr<AnimatedModel> animModel = aGO->GetComponent<AnimatedModel>();
	if (model)
	{
		model->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>(aAssetPath)->material);
	}
	else if (animModel)
	{
		animModel->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>(aAssetPath)->material);
	}
	else
	{
		LOG(LogApplication, Warning, "No model exists to apply material to!");
		myLogs.emplace_back("[ERROR] No model exists to apply material to!");
		return;
	}

	myLogs.emplace_back("[LOG] Set material to " + aAssetPath.filename().stem().string());
}

void ModelViewer::SetTexture(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	std::string assetName = aAssetPath.filename().stem().string();
	std::shared_ptr<Model> model = aGO->GetComponent<Model>();
	std::shared_ptr<AnimatedModel> animModel = aGO->GetComponent<AnimatedModel>();
	if (model)
	{
		if (model->GetMaterialOnSlot(0) == AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material)
		{
			std::shared_ptr<Material> newMat = std::make_shared<Material>();
			newMat->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>("Default_C")->texture);
			newMat->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>("Default_N")->texture);
			newMat->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>("Default_M")->texture);
			newMat->MaterialSettings().albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
			model->SetMaterialOnSlot(0, newMat);
		}

		std::shared_ptr<Material> modelMat = model->GetMaterialOnSlot(0);

		if (assetName.ends_with("C") || assetName.ends_with("c"))
		{
			modelMat->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
			myLogs.emplace_back("[LOG] Set albedo texture to " + aAssetPath.filename().stem().string());
		}
		else if (assetName.ends_with("N") || assetName.ends_with("n"))
		{
			modelMat->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
			myLogs.emplace_back("[LOG] Set normal texture to " + aAssetPath.filename().stem().string());
		}
		else if (assetName.ends_with("M") || assetName.ends_with("m"))
		{
			modelMat->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
			myLogs.emplace_back("[LOG] Set material texture to " + aAssetPath.filename().stem().string());
		}
		else
		{
			LOG(LogApplication, Warning, "No recognized filename suffix detected!");
			myLogs.emplace_back("[ERROR] No recognized filename suffix detected!");
		}
	}
	else if (animModel)
	{
		if (animModel->GetMaterialOnSlot(0) == AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material)
		{
			std::shared_ptr<Material> newMat = std::make_shared<Material>();
			animModel->SetMaterialOnSlot(0, newMat);
		}

		std::shared_ptr<Material> modelMat = animModel->GetMaterialOnSlot(0);

		if (assetName.ends_with("C"))
		{
			modelMat->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
			myLogs.emplace_back("[LOG] Set albedo texture to " + aAssetPath.filename().stem().string());
		}
		else if (assetName.ends_with("N"))
		{
			modelMat->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
			myLogs.emplace_back("[LOG] Set normal texture to " + aAssetPath.filename().stem().string());
		}
		else if (assetName.ends_with("M"))
		{
			modelMat->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
			myLogs.emplace_back("[LOG] Set material texture to " + aAssetPath.filename().stem().string());
		}
	}
	else
	{
		LOG(LogApplication, Warning, "No model exists to apply texture to!");
		myLogs.emplace_back("[ERROR] No model exists to apply texture to!");
	}
}
