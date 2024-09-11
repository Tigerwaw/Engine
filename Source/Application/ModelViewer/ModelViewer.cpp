#include "ModelViewer.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new ModelViewer();
}

void ModelViewer::InitializeApplication()
{
	myLogs.emplace_back("[LOG] Started ModelViewer");
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	GraphicsEngine::Get().DrawGizmos = true;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_ModelViewerScene.json");
	std::shared_ptr<GameObject> newGO = std::make_shared<GameObject>();
	newGO->SetName("Model");
	newGO->AddComponent<Transform>(CU::Vector3f(0, 0, 0), CU::Vector3f(0, -180.0f, 0));
	Engine::GetInstance().GetSceneHandler().Instantiate(newGO);

	std::shared_ptr<Transform> camTransform = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Transform>();
	cameraStartingPos = camTransform->GetTranslation();
	cameraStartingRot = camTransform->GetRotation();

	aLightStartIntensity = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("A_Light")->GetComponent<AmbientLight>()->GetIntensity();

	std::shared_ptr<GameObject> dLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("D_Light");
	dLightStartingRot = dLight->GetComponent<Transform>()->GetRotation();
	dLightStartIntensity = dLight->GetComponent<DirectionalLight>()->GetIntensity();

	myMaterial = std::make_shared<Material>();
	myPSO = std::make_shared<PipelineStateObject>();
	ResetMaterial();

	SetupImguiStyle();

	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();
	inputHandler.RegisterBinaryAction("W", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("A", Keys::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("S", Keys::S, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("D", Keys::D, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("F6", Keys::F6, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("F7", Keys::F7, GenericInput::ActionType::Clicked);
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
				
				AssetManager::Get().RegisterAsset(filePath);
				std::filesystem::path assetPath = AssetManager::Get().MakeRelative(filePath);
				if (assetPath == "")
				{
					LOG(LogApplication, Error, "Couldn't find asset path!");
					myLogs.emplace_back("[ERROR] Couldn't find asset path!", CU::Vector3f(1.0f, 0, 0));
					return;
				}

				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				if (!go)
				{
					LOG(LogApplication, Error, "Couldn't find game object");
					myLogs.emplace_back("[ERROR] Couldn't find game object!", CU::Vector3f(1.0f, 0, 0));
				}

				const std::string assetExt = assetPath.extension().string();
				const std::string assetName = assetPath.filename().stem().string();
				if (assetExt == ".fbx")
				{
					if (assetName.starts_with("SM") || assetName.starts_with("sm"))
					{
						SetModel(go, assetPath);
					}
					else if (assetName.starts_with("SK") || assetName.starts_with("sk"))
					{
						SetAnimatedModel(go, assetPath);
					}
					else if (assetName.starts_with("A") || assetName.starts_with("a"))
					{
						SetAnimation(go, assetPath);
					}
					else
					{
						LOG(LogApplication, Error, "A file of type '.fbx' must be prefixed by 'SM_' or 'SK_'");
						myLogs.emplace_back("[ERROR] A file of type '.fbx' must be prefixed by 'SM_' or 'SK_'", CU::Vector3f(1.0f, 0, 0));
					}
				}
				else if (assetExt == ".dds")
				{
					if (assetName.starts_with("T"))
					{
						SetTexture(go, assetPath);
					}
					else
					{
						LOG(LogApplication, Error, "A file of type '.dds' must be prefixed by 'T_'");
						myLogs.emplace_back("[ERROR] A file of type '.dds' must be prefixed by 'T_'", CU::Vector3f(1.0f, 0, 0));
					}
				}
				else if (assetExt == ".json")
				{
					if (assetName.starts_with("MAT") || assetName.starts_with("mat"))
					{
						SetMaterial(go, assetPath);
					}
					else if (assetName.starts_with("PSO") || assetName.starts_with("pso"))
					{
						SetPSO(assetPath);
					}
					else
					{
						LOG(LogApplication, Error, "A file of type '.json' must be prefixed by 'MAT_' or 'PSO_'");
						myLogs.emplace_back("[ERROR] A file of type '.json' must be prefixed by 'MAT_' or 'PSO_'", CU::Vector3f(1.0f, 0, 0));
					}
				}
				else if (assetExt == ".cso")
				{
					if (assetName.starts_with("SH") || assetName.starts_with("sh"))
					{
						myLogs.emplace_back("[ERROR] Modelviewer does not support loading in individual shaders currently!", CU::Vector3f(1.0f, 0, 0));
						//SetShader(assetPath);
					}
					else
					{
						LOG(LogApplication, Error, "A file of type '.cso' must be prefixed by 'SH_'");
						myLogs.emplace_back("[ERROR] A file of type '.cso' must be prefixed by 'SH_'", CU::Vector3f(1.0f, 0, 0));
					}
				}
				else
				{
					LOG(LogApplication, Error, "Can't load filetype {}", assetExt.c_str());
					myLogs.emplace_back("[ERROR] Can't load filetype " + assetExt + "!", CU::Vector3f(1.0f, 0, 0));
				}
			}

			DragFinish(hDrop);
		});


	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this]()
		{
#ifndef _RETAIL
			CU::Vector2f size(250.0f, 220.0f);
			float offset = 15.0f;
			CU::Vector2f windowPos = Engine::GetInstance().GetApplicationWindow().GetTopLeft();
			windowPos.x -= size.x + offset;

			ImGui::SetNextWindowPos({ windowPos.x, windowPos.y });
			ImGui::SetNextWindowContentSize({ size.x, size.y });
			ImGui::PushFont(newFont);

			bool* open = NULL;
			ImGui::Begin("Modelviewer", open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

			// Rendering
			{
				ImGui::Text("Rendering Mode");
				if (ImGui::BeginCombo("##RenderModeDropdown", debugModeNames[static_cast<int>(GraphicsEngine::Get().GetCurrentDebugMode())].c_str()))
				{
					for (unsigned i = 0; i < static_cast<unsigned>(DebugMode::COUNT); i++)
					{
						if (ImGui::Selectable(debugModeNames[i].c_str())) GraphicsEngine::Get().SetDebugMode(static_cast<DebugMode>(i));
					}

					ImGui::EndCombo();
				}
			}

			currentDebugMode = static_cast<unsigned>(GraphicsEngine::Get().GetCurrentDebugMode());

			// Tonemapping
			{
				ImGui::Text("Tonemapper");
				if (ImGui::BeginCombo("##TonemapperDropdown", tonemapperNames[static_cast<int>(GraphicsEngine::Get().GetTonemapper())].c_str()))
				{
					for (unsigned i = 0; i < static_cast<unsigned>(Tonemapper::COUNT); i++)
					{
						if (ImGui::Selectable(tonemapperNames[i].c_str())) GraphicsEngine::Get().SetTonemapper(static_cast<Tonemapper>(i));
					}

					ImGui::EndCombo();
				}
			}

			currentTonemapper = static_cast<unsigned>(GraphicsEngine::Get().GetTonemapper());

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

			if (ImGui::Button("Set Cube Primitive"))
			{
				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				std::filesystem::path prim = "SM_CubePrimitive";
				SetModel(go, prim);
				std::shared_ptr<Transform> transform = go->GetComponent<Transform>();
				transform->SetScale(50.0f, 50.0f, 50.0f);
				transform->SetTranslation(0, 100.0f, 0);
			};

			if (ImGui::Button("Set Sphere Primitive"))
			{
				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				std::filesystem::path prim = "EngineAssets/Models/SM_Sphere.fbx";
				SetModel(go, prim);
				std::shared_ptr<Transform> transform = go->GetComponent<Transform>();
				transform->SetTranslation(0, 100.0f, 0);
				transform->SetScale(50.0f, 50.0f, 50.0f);
			};

			if (ImGui::Button("Set Plane Primitive"))
			{
				std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
				std::filesystem::path prim = "SM_PlanePrimitive";
				SetModel(go, prim);
				std::shared_ptr<Transform> transform = go->GetComponent<Transform>();
				transform->SetRotation(90.0f, -180.0f, 0);
				transform->SetScale(50.0f, 50.0f, 50.0f);
				transform->SetTranslation(0, 100.0f, 0);
			}

			ImGui::End();
			ImGui::PopFont();
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this]()
		{
#ifndef _RETAIL
			CU::Vector2f size(250.0f, 440.0f);
			float offset = 15.0f;
			CU::Vector2f windowPos = Engine::GetInstance().GetApplicationWindow().GetTopLeft();
			windowPos.x -= size.x + offset;
			windowPos.y += 270.0f;

			ImGui::SetNextWindowPos({ windowPos.x, windowPos.y });
			ImGui::SetNextWindowContentSize({ size.x, size.y });
			ImGui::PushFont(newFont);
			bool* open = NULL;
			ImGui::Begin("Lighting Settings", open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
			{
				if (ImGui::BeginTabBar("Lights"))
				{
					if (ImGui::BeginTabItem("Ambient Light"))
					{
						std::shared_ptr<GameObject> ambientLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("A_Light");
						bool active = ambientLight->GetActive();
						if (ImGui::Checkbox("Set Active", &active)) ambientLight->SetActive(active);
						
						std::shared_ptr<AmbientLight> aLight = ambientLight->GetComponent<AmbientLight>();
						float intensity = aLight->GetIntensity();
						ImGui::Text("Intensity");
						if (ImGui::SliderFloat("##AmbientIntensity", &intensity, 0, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) aLight->SetIntensity(intensity);
						ImGui::Spacing();
						float color[3] = { aLight->GetColor().x, aLight->GetColor().y, aLight->GetColor().z };
						ImGui::Text("Color");
						unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
						if (ImGui::ColorPicker3("##AmbientColor", color, flags)) aLight->SetColor({ color[0], color[1], color[2] });
						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Directional Light"))
					{
						std::shared_ptr<GameObject> directionalLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("D_Light");
						bool active = directionalLight->GetActive();
						if (ImGui::Checkbox("Set Active", &active)) directionalLight->SetActive(active);

						std::shared_ptr<DirectionalLight> dLight = directionalLight->GetComponent<DirectionalLight>();
						float intensity = dLight->GetIntensity();
						ImGui::Text("Intensity");
						if (ImGui::SliderFloat("##DirectionalIntensity", &intensity, 0, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) dLight->SetIntensity(intensity);
						ImGui::Spacing();
						float color[3] = { dLight->GetColor().x, dLight->GetColor().y, dLight->GetColor().z };
						ImGui::Text("Color");
						unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
						if (ImGui::ColorPicker3("##DirectionalColor", color, flags)) dLight->SetColor({ color[0], color[1], color[2] });
						ImGui::Spacing();
						
						std::shared_ptr<GameObject> dLightParent = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("D_Light_Parent");
						std::shared_ptr<Transform> dlptransform = dLightParent->GetComponent<Transform>();
						std::shared_ptr<Transform> dlightTransform = directionalLight->GetComponent<Transform>();
						CU::Vector3f dlpRot = dlptransform->GetRotation();
						CU::Vector3f dlightRot = dlightTransform->GetRotation();
						ImGui::Text("Rotation");
						ImGui::Text("Pitch:");
						ImGui::SameLine();
						if (ImGui::SliderFloat("##DLightRotX", &dlightRot.x, 0, 90.0f, "%.0f")) dlightTransform->SetRotation(dlightRot);
						ImGui::Text("Yaw:");
						ImGui::SameLine();
						if (ImGui::SliderFloat("##DLightRotY", &dlpRot.y, 0, 360.0f, "%.0f")) dlptransform->SetRotation(dlpRot);
						ImGui::EndTabItem();
					}

					std::shared_ptr<AnimatedModel> animModel = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model")->GetComponent<AnimatedModel>();
					if (animModel)
					{
						if (ImGui::BeginTabItem("Animations", 0, switchToAnimationTab ? ImGuiTabItemFlags_SetSelected : 0))
						{
							ImGui::SliderFloat("Blendtime", &currentBlendtime, 0, 5.0f);

							ImVec4 buttonColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
							ImVec4 hoveredButtonColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
							CU::Vector3f buttonColorCU = { buttonColor.x, buttonColor.y, buttonColor.z };
							CU::Vector3f hoveredButtonColorCU = { hoveredButtonColor.x, hoveredButtonColor.y, hoveredButtonColor.z };

							std::vector<std::shared_ptr<AnimatedModel::AnimationState>> states = animModel->GetAnimationStatesOnLayer(0);
							for (auto& state : states)
							{
								ImVec4 selectedAnimColor = buttonColor;
								
								if (state->name == currentAnimName)
								{
									selectedAnimColor = hoveredButtonColor;

									if (animModel->IsLayerCurrentlyBlending(0))
									{
										float blendFactor = animModel->GetCurrentBlendFactorOnLayer(0);

										CU::Vector3f lerpedColor = CU::Vector3f::Lerp(buttonColorCU, hoveredButtonColorCU, blendFactor);

										selectedAnimColor = { lerpedColor.x, lerpedColor.y, lerpedColor.z, 1.0f };
									}
								}
								else if (state->name == previousAnimName)
								{
									selectedAnimColor = buttonColor;

									if (animModel->IsLayerCurrentlyBlending(0))
									{
										float blendFactor = animModel->GetCurrentBlendFactorOnLayer(0);

										CU::Vector3f lerpedColor = CU::Vector3f::Lerp(buttonColorCU, hoveredButtonColorCU, 1 - blendFactor);

										selectedAnimColor = { lerpedColor.x, lerpedColor.y, lerpedColor.z, 1.0f };
									}
								}

								ImGui::PushStyleColor(ImGuiCol_Button, selectedAnimColor);
								if (ImGui::Button(state->name.c_str()))
								{
									previousAnimName = currentAnimName;
									currentAnimName = state->name;
									animModel->SetCurrentAnimationOnLayer(state->name.c_str(), "", currentBlendtime);
								}
								ImGui::PopStyleColor();
							}

							ImGui::EndTabItem();
						}
					}

					ImGui::EndTabBar();
				}
			}

			ImGui::End();
			ImGui::PopFont();
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this]()
		{
#ifndef _RETAIL
			CU::Vector2f size(300.0f, 200.0f);
			CU::Vector2f windowPos = Engine::GetInstance().GetApplicationWindow().GetTopRight();

			ImGui::SetNextWindowPos({ windowPos.x, windowPos.y });
			ImGui::SetNextWindowContentSize({ size.x, size.y });
			ImGui::PushFont(newFont);
			bool* open = NULL;
			ImGui::Begin("Log", open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
			{
				if (myLogs.size() > 0)
				{
					for (size_t i = myLogs.size() - 1; i > 0; i--)
					{
						ImGui::PushTextWrapPos(280.0f);
						ImGui::TextColored({ myLogs[i].color.x, myLogs[i].color.y, myLogs[i].color.z, 1.0f }, myLogs[i].message.c_str());
						if (myLogs[i].tooltip != "")
						{
							ImGui::SetItemTooltip(myLogs[i].tooltip.c_str());
						}
						ImGui::PopTextWrapPos();
					}
				}
			}
			ImGui::End();
			ImGui::PopFont();
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this]()
		{
#ifndef _RETAIL
			CU::Vector2f size(300.0f, 440.0f);
			CU::Vector2f windowPos = Engine::GetInstance().GetApplicationWindow().GetTopRight();
			windowPos.y += 250.0f;

			CU::Vector2f textureSize(64.0f, 64.0f);
			CU::Vector2f textureHoverSize(256.0f, 256.0f);

			ImGui::SetNextWindowPos({ windowPos.x, windowPos.y });
			ImGui::SetNextWindowContentSize({ size.x, size.y });
			ImGui::PushFont(newFont);
			bool* open = NULL;
			ImGui::Begin("Model Settings", open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
			{
				ImGui::Text(std::string("Mesh - " + myMeshName).c_str());
				ImGui::SetItemTooltip(myMeshPath.c_str());
				ImGui::Spacing();

				if (ImGui::BeginTabBar("Settings"))
				{
					if (ImGui::BeginTabItem("Material"))
					{
						if (myIsCustomMaterial)
						{
							ImGui::InputText("Material Name", &myMaterialName);
						}
						else
						{
							ImGui::Text(std::string("Material - " + myMaterialName).c_str());
							ImGui::SetItemTooltip(myMaterialPath.c_str());
						}

						if (ImGui::Button("Reset Material"))
						{
							ResetMaterial();
						};

						if (myIsCustomMaterial)
						{
							ImGui::SameLine();
							if (ImGui::Button("Export as new material"))
							{
								ExportMaterial();
							};
						}

						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::Text(std::string("Albedo Texture - " + myAlbedoTexName).c_str());
						ImGui::SetItemTooltip(myAlbedoTexPath.c_str());
						ImGui::Image((void*)myMaterial->GetAlbedoTexture().GetSRV(), { textureSize.x, textureSize.y });
						if (ImGui::BeginItemTooltip())
						{
							ImGui::Image((void*)myMaterial->GetAlbedoTexture().GetSRV(), { textureHoverSize.x, textureHoverSize.y });
							ImGui::EndTooltip();
						}
						ImGui::SameLine();
						if (ImGui::Button("Remove##Albedo"))
						{
							myMaterial->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>("T_Default_C")->texture);
						}
						ImGui::Spacing();

						ImGui::Text(std::string("Normal Texture - " + myNormalTexName).c_str());
						ImGui::SetItemTooltip(myNormalTexPath.c_str());
						ImGui::Image((void*)myMaterial->GetNormalTexture().GetSRV(), { textureSize.x, textureSize.y });
						if (ImGui::BeginItemTooltip())
						{
							ImGui::Image((void*)myMaterial->GetNormalTexture().GetSRV(), { textureHoverSize.x, textureHoverSize.y });
							ImGui::EndTooltip();
						}
						ImGui::SameLine();
						if (ImGui::Button("Remove##Normal"))
						{
							myMaterial->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>("T_Default_N")->texture);
						}
						ImGui::Spacing();

						ImGui::Text(std::string("Material Texture - " + myMaterialTexName).c_str());
						ImGui::SetItemTooltip(myMaterialTexPath.c_str());
						ImGui::Image((void*)myMaterial->GetMaterialTexture().GetSRV(), { textureSize.x, textureSize.y });
						if (ImGui::BeginItemTooltip())
						{
							ImGui::Image((void*)myMaterial->GetMaterialTexture().GetSRV(), { textureHoverSize.x, textureHoverSize.y });
							ImGui::EndTooltip();
						}
						ImGui::SameLine();
						if (ImGui::Button("Remove##Material"))
						{
							myMaterial->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>("T_Default_M")->texture);
						}

						ImGui::EndTabItem();
					}

					/*if (ImGui::BeginTabItem("PSO"))
					{
						if (myIsCustomMaterial)
						{
							ImGui::InputText("PSO Name", &myPSOName);
						}
						else
						{
							ImGui::Text(std::string("PSO - " + myPSOName).c_str());
							ImGui::SetItemTooltip(myMaterialPath.c_str());
						}

						if (ImGui::Button("Reset PSO"))
						{
							ResetPSO();
						};

						if (myIsCustomMaterial)
						{
							ImGui::SameLine();
							ImGui::BeginDisabled(true);
							if (ImGui::Button("Export as new PSO"))
							{
								ExportPSO();
							};
							ImGui::EndDisabled();
						}

						ImGui::Text(std::string("Vertex Shader - " + myVertexShaderName).c_str());
						ImGui::SetItemTooltip(myVertexShaderPath.c_str());

						ImGui::Text(std::string("Geometry Shader - " + myGeometryShaderName).c_str());
						ImGui::SetItemTooltip(myGeometryShaderPath.c_str());

						ImGui::Text(std::string("Pixel Shader - " + myPixelShaderName).c_str());
						ImGui::SetItemTooltip(myPixelShaderPath.c_str());

						ImGui::EndTabItem();
					}*/

					ImGui::EndTabBar();
				}
			}
			ImGui::End();
			ImGui::PopFont();
#endif
		});
}

void ModelViewer::UpdateApplication()
{
	switchToAnimationTab = false;

	if (Engine::GetInstance().GetInputHandler().GetBinaryAction("F6"))
	{
		currentDebugMode += 1;
		if (currentDebugMode >= static_cast<unsigned>(DebugMode::COUNT))
		{
			currentDebugMode = 0;
		}

		GraphicsEngine::Get().SetDebugMode(static_cast<DebugMode>(currentDebugMode));
	}

	if (Engine::GetInstance().GetInputHandler().GetBinaryAction("F7"))
	{
		currentTonemapper += 1;
		if (currentTonemapper >= static_cast<unsigned>(Tonemapper::COUNT))
		{
			currentTonemapper = 0;
		}

		GraphicsEngine::Get().SetTonemapper(static_cast<Tonemapper>(currentTonemapper));
	}
}

void ModelViewer::ResetScene()
{
	Engine& engine = Engine::GetInstance();

	ResetPSO();
	ResetMaterial();

	std::shared_ptr<GameObject> go = engine.GetSceneHandler().FindGameObjectByName("Model");
	ResetGameObject(go);

	std::shared_ptr<GameObject> aLight = engine.GetSceneHandler().FindGameObjectByName("A_Light");
	std::shared_ptr<AmbientLight> aLightComp = aLight->GetComponent<AmbientLight>();
	aLightComp->SetColor(CU::Vector3f(1.0f, 1.0f, 1.0f));
	aLightComp->SetIntensity(aLightStartIntensity);
	aLight->SetActive(true);

	engine.GetSceneHandler().FindGameObjectByName("D_Light_Parent")->GetComponent<Transform>()->SetRotation(0, 0, 0);

	std::shared_ptr<GameObject> dLight = engine.GetSceneHandler().FindGameObjectByName("D_Light");
	std::shared_ptr<DirectionalLight> dLightComp = dLight->GetComponent<DirectionalLight>();
	dLightComp->SetColor(CU::Vector3f(1.0f, 1.0f, 1.0f));
	dLightComp->SetIntensity(dLightStartIntensity);
	dLight->GetComponent<Transform>()->SetRotation(dLightStartingRot);
	dLight->SetActive(true);

	std::shared_ptr<Transform> camTransform = engine.GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Transform>();
	camTransform->SetTranslation(cameraStartingPos);
	camTransform->SetRotation(cameraStartingRot);

	currentDebugMode = 0;
	GraphicsEngine::Get().SetDebugMode(DebugMode::None);
	currentTonemapper = 0;
	GraphicsEngine::Get().SetTonemapper(Tonemapper::UE);
	currentBlendtime = 0.5f;

	engine.GetSceneHandler().FindGameObjectByName("Plane")->SetActive(true);

	myLogs.clear();
	myLogs.emplace_back("[LOG] Reset Scene");
}

void ModelViewer::ResetGameObject(std::shared_ptr<GameObject> aGO)
{
	std::shared_ptr<Transform> transform = aGO->GetComponent<Transform>();
	transform->SetTranslation(0, 0, 0);
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

void ModelViewer::ResetMaterial()
{
	AssetManager& am = AssetManager::Get();
	std::shared_ptr<Material> defaultMat = am.GetAsset<MaterialAsset>("MAT_DefaultMaterial")->material;
	myMaterial->SetPSO(defaultMat->GetPSO());
	myMaterial->MaterialSettings().albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	myMaterial->SetAlbedoTexture(am.GetAsset<TextureAsset>("T_Default_C")->texture);
	myMaterial->SetNormalTexture(am.GetAsset<TextureAsset>("T_Default_N")->texture);
	myMaterial->SetMaterialTexture(am.GetAsset<TextureAsset>("T_Default_M")->texture);

	myMaterialName = "MAT_DefaultMaterial";
	myMaterialPath = "";
	myAlbedoTexName = "T_Default_C";
	myAlbedoTexPath = "";
	myNormalTexName = "T_Default_N";
	myNormalTexPath = "";
	myMaterialTexName = "T_Default_M";
	myMaterialTexPath = "";
	myIsCustomMaterial = false;

	myLogs.emplace_back("[LOG] Reset material to default");
}

void ModelViewer::ResetPSO()
{
	std::shared_ptr<PipelineStateObject> defaultPSO = GraphicsEngine::Get().GetDefaultPSO();
	myPSO = defaultPSO;

	myPSOName = "Default";
	myPSOPath = "";
	myVertexShaderName = "";
	myVertexShaderPath = "";
	myGeometryShaderName = "";
	myGeometryShaderPath = "";
	myPixelShaderName = "";
	myPixelShaderPath = "";

	myLogs.emplace_back("[LOG] Reset PSO to default");
}

void ModelViewer::SetModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	ResetGameObject(aGO);
	aGO->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>(aAssetPath)->mesh, myMaterial);
	std::string assetName = aAssetPath.filename().stem().string();
	myMeshName = assetName;
	myMeshPath = aAssetPath.string();
	myLogs.emplace_back("[LOG] Model set to " + assetName);
}

void ModelViewer::SetAnimatedModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	ResetGameObject(aGO);
	aGO->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>(aAssetPath)->mesh, myMaterial);
	aGO->GetComponent<AnimatedModel>()->StopAnimation();
	std::string assetName = aAssetPath.filename().stem().string();
	myMeshName = assetName;
	myMeshPath = aAssetPath.string();
	myLogs.emplace_back("[LOG] Model set to " + assetName);
}

void ModelViewer::SetAnimation(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	std::shared_ptr<AnimatedModel> animModel = aGO->GetComponent<AnimatedModel>();
	if (!animModel)
	{
		LOG(LogApplication, Warning, "The currently loaded model does not support animations!");
		myLogs.emplace_back("[ERROR] The currently loaded model does not support animations!", CU::Vector3f(1.0f, 0, 0));
		return;
	}

	switchToAnimationTab = true;

	std::string animName = aAssetPath.filename().stem().string();
	animModel->AddAnimationToLayer(animName, AssetManager::Get().GetAsset<AnimationAsset>(aAssetPath)->animation, "", true);
	animModel->SetCurrentAnimationOnLayer(animName, "", 0, 0);
	animModel->PlayAnimation();
	myLogs.emplace_back("[LOG] Added animation " + aAssetPath.filename().stem().string());
}

void ModelViewer::SetMaterial(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	myMaterial = AssetManager::Get().GetAsset<MaterialAsset>(aAssetPath)->material->CreateInstance();
	std::string assetName = aAssetPath.filename().stem().string();
	myMaterialName = assetName;
	myMaterialPath = aAssetPath.string();

	std::ifstream path(AssetManager::Get().GetContentRoot() / aAssetPath);
	nl::json data = nl::json();

	try
	{
		data = nl::json::parse(path);
	}
	catch (nl::json::parse_error e)
	{
		LOG(LogApplication, Error, "Failed to read material asset {}, {}", aAssetPath.filename().string(), e.what());
		return;
	}
	path.close();

	if (data.contains("AlbedoTexture"))
	{
		std::filesystem::path albedoPath = data["AlbedoTexture"].get<std::string>();
		myAlbedoTexName = albedoPath.filename().stem().string();
		myAlbedoTexPath = albedoPath.string();
	}

	if (data.contains("NormalTexture"))
	{
		std::filesystem::path normalPath = data["NormalTexture"].get<std::string>();
		myNormalTexName = normalPath.filename().stem().string();
		myNormalTexPath = normalPath.string();
	}

	if (data.contains("MaterialTexture"))
	{
		std::filesystem::path materialPath = data["MaterialTexture"].get<std::string>();
		myMaterialTexName = materialPath.filename().stem().string();
		myMaterialTexPath = materialPath.string();
	}

	std::shared_ptr<Model> model = aGO->GetComponent<Model>();
	std::shared_ptr<AnimatedModel> animModel = aGO->GetComponent<AnimatedModel>();
	if (model)
	{
		model->SetMaterialOnSlot(0, myMaterial);
	}
	else if (animModel)
	{
		animModel->SetMaterialOnSlot(0, myMaterial);
	}

	myLogs.emplace_back("[LOG] Set material to " + assetName);
	myIsCustomMaterial = false;
}

void ModelViewer::SetTexture(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath)
{
	std::string assetName = aAssetPath.filename().stem().string();

	if (assetName.ends_with("C") || assetName.ends_with("c"))
	{
		myMaterial->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
		myAlbedoTexName = assetName;
		myAlbedoTexPath = aAssetPath.string();
		myLogs.emplace_back("[LOG] Set albedo texture to " + assetName);
		myIsCustomMaterial = true;
		myMaterialName = "";
		myMaterialPath = "";
	}
	else if (assetName.ends_with("N") || assetName.ends_with("n"))
	{
		myMaterial->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
		myNormalTexName = assetName;
		myNormalTexPath = aAssetPath.string();
		myLogs.emplace_back("[LOG] Set normal texture to " + assetName);
		myIsCustomMaterial = true;
		myMaterialName = "";
		myMaterialPath = "";
	}
	else if (assetName.ends_with("M") || assetName.ends_with("m"))
	{
		myMaterial->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>(aAssetPath)->texture);
		myMaterialTexName = assetName;
		myMaterialTexPath = aAssetPath.string();
		myLogs.emplace_back("[LOG] Set material texture to " + assetName);
		myIsCustomMaterial = true;
		myMaterialName = "";
		myMaterialPath = "";
	}
	else
	{
		LOG(LogApplication, Warning, "No recognized texture suffix detected!");
		myLogs.emplace_back("[ERROR] No recognized texture suffix detected! (Hover for more info)", "A texture used in a material needs to be ended with _C for basecolor, _N for normal, or _M for a material texture", CU::Vector3f(1.0f, 0, 0));
	}
}

void ModelViewer::SetPSO(std::filesystem::path& aAssetPath)
{
	myPSO = AssetManager::Get().GetAsset<PSOAsset>(aAssetPath.stem())->pso;
	std::string assetName = aAssetPath.stem().string();
	myPSOName = assetName;
	myPSOPath = aAssetPath.string();

	std::ifstream path(AssetManager::Get().GetContentRoot() / aAssetPath);
	nl::json data = nl::json();

	try
	{
		data = nl::json::parse(path);
	}
	catch (nl::json::parse_error e)
	{
		LOG(LogApplication, Error, "Failed to read pso asset {}, {}", aAssetPath.filename().string(), e.what());
		return;
	}
	path.close();

	if (data.contains("VertexShader"))
	{
		std::filesystem::path vsPath = data["VertexShader"].get<std::string>();
		myVertexShaderName = vsPath.filename().stem().string();
		myVertexShaderPath = vsPath.string();
	}

	if (data.contains("GeometryShader"))
	{
		std::filesystem::path gsPath = data["GeometryShader"].get<std::string>();
		myGeometryShaderName = gsPath.filename().stem().string();
		myGeometryShaderPath = gsPath.string();
	}

	if (data.contains("PixelShader"))
	{
		std::filesystem::path psPath = data["PixelShader"].get<std::string>();
		myPixelShaderName = psPath.filename().stem().string();
		myPixelShaderPath = psPath.string();
	}

	myMaterial->SetPSO(myPSO);

	myLogs.emplace_back("[LOG] Set PSO to " + assetName);
}

void ModelViewer::SetShader(std::filesystem::path& aAssetPath)
{
	std::shared_ptr<ShaderAsset> shaderAsset = AssetManager::Get().GetAsset<ShaderAsset>(aAssetPath.filename());
	if (!shaderAsset)
	{
		LOG(LogApplication, Warning, "Could not find asset at path {}!", aAssetPath.string());
		myLogs.emplace_back("[ERROR] Could not find asset at path " + aAssetPath.string(), CU::Vector3f(1.0f, 0, 0));
		return;
	}

	if (shaderAsset->name.string().ends_with("VS"))
	{
		myPSO->VertexShader = shaderAsset->shader;
		myMaterial->SetPSO(myPSO);
		myVertexShaderName = shaderAsset->name.string();
		myVertexShaderPath = shaderAsset->path.string();
	}
	else if (shaderAsset->name.string().ends_with("GS"))
	{
		myPSO->GeometryShader = shaderAsset->shader;
		myMaterial->SetPSO(myPSO);
		myGeometryShaderName = shaderAsset->name.string();
		myGeometryShaderPath = shaderAsset->path.string();
	}
	else if (shaderAsset->name.string().ends_with("PS"))
	{
		myPSO->PixelShader = shaderAsset->shader;
		myMaterial->SetPSO(myPSO);
		myPixelShaderName = shaderAsset->name.string();
		myPixelShaderPath = shaderAsset->path.string();
	}
	else
	{
		LOG(LogApplication, Warning, "No recognized filename suffix detected!");
		myLogs.emplace_back("[ERROR] No recognized filename suffix detected!", CU::Vector3f(1.0f, 0, 0));
	}
}

void ModelViewer::ExportMaterial()
{
	nl::json newMat;

	newMat["PSO"] = std::string("PSO_PBR");
	std::unordered_map<std::string, float> albedoTint;
	albedoTint.emplace("R", 1.0f);
	albedoTint.emplace("G", 1.0f);
	albedoTint.emplace("B", 1.0f);
	albedoTint.emplace("A", 1.0f);
	newMat["AlbedoTint"] = albedoTint;
	newMat["AlbedoTexture"] = myAlbedoTexPath;
	newMat["NormalTexture"] = myNormalTexPath;
	newMat["MaterialTexture"] = myMaterialTexPath;

	std::string name = "MAT_" + myMaterialName + ".json";
	std::filesystem::path path = AssetManager::Get().GetContentRoot() / "Materials" / name;
	std::ofstream outFile(path);
	outFile << newMat;
	outFile.close();

	myLogs.emplace_back("[LOG] Exported " + name, path.string());
}

void ModelViewer::ExportPSO()
{
	nl::json newPSO;

	newPSO["VertexType"] = std::string("Default");
	newPSO["VertexShader"] = myVertexShaderPath;
	newPSO["GeometryShader"] = myGeometryShaderPath;
	newPSO["PixelShader"] = myPixelShaderPath;

	std::string name = "PSO_" + myPSOName + ".json";
	std::filesystem::path path = AssetManager::Get().GetContentRoot() / "EngineAssets/PSOs" / name;
	std::ofstream outFile(path);
	outFile << newPSO;
	outFile.close();

	myLogs.emplace_back("[LOG] Exported " + name, path.string());
}

void ModelViewer::SetupImguiStyle()
{
#ifndef _RETAIL
	std::filesystem::path fontPath = AssetManager::Get().GetContentRoot() / "Fonts/Roboto-Regular.ttf";
	newFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16.0f);
	ImGui::GetIO().Fonts->Build();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.WindowBorderSize = 0;
	style.FrameRounding = 2.0f;
	style.GrabRounding = 2.0f;
	style.TabRounding = 2.0f;
	style.FrameBorderSize = 0;
	style.GrabMinSize = 8.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.32f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.56f, 0.00f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.12f, 0.05f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.17f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.18f, 0.08f, 0.94f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.34f, 0.19f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.34f, 0.47f, 0.20f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.34f, 0.60f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.30f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.49f, 0.31f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.16f, 0.08f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.09f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.20f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.48f, 0.27f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.60f, 0.30f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.63f, 0.22f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 1.00f, 0.32f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.54f, 0.34f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.56f, 0.36f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.23f, 0.35f, 0.24f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.31f, 0.53f, 0.44f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.69f, 0.44f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.15f, 0.36f, 0.16f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.56f, 0.35f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.60f, 0.62f, 0.52f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.06f, 0.53f, 0.27f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.02f, 0.63f, 0.32f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.51f, 0.22f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.63f, 0.22f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.80f, 0.28f, 0.95f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.70f, 0.34f, 0.80f);
	colors[ImGuiCol_Tab] = ImVec4(0.04f, 0.33f, 0.21f, 0.86f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.01f, 0.51f, 0.11f, 1.00f);
	colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.60f, 1.00f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.20f, 0.09f, 0.97f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.33f, 0.15f, 1.00f);
	colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.39f, 0.54f, 0.18f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.18f, 0.68f, 0.36f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.06f, 0.31f, 0.10f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.31f, 0.65f, 0.37f, 1.00f);
#endif
}