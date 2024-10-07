#include "FeatureShowcase.h"
#include <GameEngine/Engine.h>
#include <GameEngine/Time/Timer.h>
#include <Psapi.h>
#include <GameEngine/ComponentSystem/GameObject.h>
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/SpotLight.h"
#include <GameEngine/ComponentSystem/Components/Graphics/Model.h>
#include <GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h>
#include <GameEngine/ComponentSystem/Components/Graphics/VFXModel.h>
#include <GameEngine/ComponentSystem/Components/Physics/Colliders/BoxCollider.h>
#include <GameEngine/ComponentSystem/Components/Physics/Colliders/SphereCollider.h>

#include <GameEngine/ComponentSystem/Components/Graphics/ParticleSystem.h>

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new FeatureShowcase();
}

void FeatureShowcase::InitializeApplication()
{
	Engine::GetInstance().GetInputHandler().SetControllerDeadZone(0.1f, 0.06f);
	Engine::GetInstance().GetAudioEngine().LoadBank("Master");
	Engine::GetInstance().GetAudioEngine().LoadBank("Master.strings");
	Engine::GetInstance().GetAudioEngine().LoadBank("Test");

	Engine::GetInstance().GetAudioEngine().AddBus(BusType::Music, "Music");
	Engine::GetInstance().GetAudioEngine().AddBus(BusType::Ambience, "Ambience");
	Engine::GetInstance().GetAudioEngine().AddBus(BusType::SFX, "SFX");

	Engine::GetInstance().GetDebugDrawer().InitializeDebugDrawer();
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_TestScene.json");

	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();
	inputHandler.RegisterBinaryAction("W", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("A", Keys::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("S", Keys::S, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("D", Keys::D, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RMB", Keys::MOUSERBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("SPACE", Keys::SPACE, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("F6", Keys::F6, GenericInput::ActionType::Clicked);
	inputHandler.RegisterBinaryAction("F7", Keys::F7, GenericInput::ActionType::Clicked);
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
	inputHandler.RegisterAnalog2DAction("LeftStick", ControllerAnalog2D::LEFT_STICK);
	inputHandler.RegisterAnalog2DAction("RightStick", ControllerAnalog2D::RIGHT_STICK);
	inputHandler.RegisterAnalogAction("LeftTrigger", ControllerAnalog::LEFT_TRIGGER);
	inputHandler.RegisterAnalogAction("RightTrigger", ControllerAnalog::RIGHT_TRIGGER);

	inputHandler.RegisterBinaryAction("SharedAction", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("SharedAction", ControllerButtons::A, GenericInput::ActionType::Held);
	
	Engine::GetInstance().GetImGuiHandler().AddNewFunction([]()
		{
#ifndef _RETAIL
			CU::Vector2f resolution = Engine::GetInstance().GetResolution();
			ImGui::SetNextWindowPos({ 0.01f * resolution.x, 0.02f * resolution.y });
			ImGui::SetNextWindowContentSize({ 0.16f * resolution.x, 0.35f * resolution.y });
			bool open = true;
			ImGui::Begin("FeatureShowcase", &open, ImGuiWindowFlags_NoSavedSettings);

			ImGui::Checkbox("Use Viewculling", &GraphicsEngine::Get().UseViewCulling);
			ImGui::Checkbox("Draw Gizmos", &GraphicsEngine::Get().DrawGizmos);
			ImGui::Checkbox("Draw Bounding Boxes", &GraphicsEngine::Get().DrawBoundingBoxes);
			ImGui::Checkbox("Draw Camera Frustums", &GraphicsEngine::Get().DrawCameraFrustums);
			ImGui::Checkbox("Draw Colliders", &GraphicsEngine::Get().DrawColliders);

			// Rendering
			{
				ImGui::Text("Rendering Mode");
				if (ImGui::BeginCombo("##RenderModeDropdown", GraphicsEngine::Get().DebugModeNames[static_cast<int>(GraphicsEngine::Get().CurrentDebugMode)].c_str()))
				{
					for (unsigned i = 0; i < static_cast<unsigned>(DebugMode::COUNT); i++)
					{
						if (ImGui::Selectable(GraphicsEngine::Get().DebugModeNames[i].c_str())) GraphicsEngine::Get().CurrentDebugMode = static_cast<DebugMode>(i);
					}
					ImGui::EndCombo();
				}
			}

			// Tonemapping
			{
				ImGui::Text("Tonemapper");
				if (ImGui::BeginCombo("##TonemapperDropdown", GraphicsEngine::Get().TonemapperNames[static_cast<int>(GraphicsEngine::Get().Tonemapper)].c_str()))
				{
					for (unsigned i = 0; i < static_cast<unsigned>(Tonemapper::COUNT); i++)
					{
						if (ImGui::Selectable(GraphicsEngine::Get().TonemapperNames[i].c_str())) GraphicsEngine::Get().Tonemapper = static_cast<Tonemapper>(i);
					}
					ImGui::EndCombo();
				}
			}

			ImGui::Checkbox("Enable Bloom", &GraphicsEngine::Get().BloomEnabled);
			ImGui::Checkbox("Enable SSAO", &GraphicsEngine::Get().SSAOEnabled);

			ImGui::Separator();

			// Animation
			{
				ImGui::Text("Animations");
				std::shared_ptr<AnimatedModel> tgaBroModel = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("TgaBro")->GetComponent<AnimatedModel>();
				if (ImGui::BeginCombo("##AnimationDropdown", tgaBroModel->GetCurrentAnimationNameOnLayer(0).c_str()))
				{
					if (ImGui::Selectable("Idle")) tgaBroModel->SetCurrentAnimationOnLayer("Idle", "", 0.5f);
					if (ImGui::Selectable("Walk")) tgaBroModel->SetCurrentAnimationOnLayer("Walk", "", 0.5f);;
					if (ImGui::Selectable("Run")) tgaBroModel->SetCurrentAnimationOnLayer("Run", "", 0.5f);;
					if (ImGui::Selectable("Wave")) tgaBroModel->SetCurrentAnimationOnLayer("Wave", "", 0.5f);;
					ImGui::EndCombo();
				}
			}

			// Test Audio
			{
				float sfxVolume = Engine::GetInstance().GetAudioEngine().GetVolumeOfBus(BusType::SFX);
				if (ImGui::SliderFloat("SFX Volume", &sfxVolume, 0, 1.0f)) Engine::GetInstance().GetAudioEngine().SetVolumeOfBus(BusType::SFX, sfxVolume);

				float musicVolume = Engine::GetInstance().GetAudioEngine().GetVolumeOfBus(BusType::Music);
				if (ImGui::SliderFloat("Music Volume", &musicVolume, 0, 1.0f)) Engine::GetInstance().GetAudioEngine().SetVolumeOfBus(BusType::Music, musicVolume);
			}

			// Test VFX
			{
				std::shared_ptr<VFXModel> vfx = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("VFXTest")->GetComponent<VFXModel>();
				if (ImGui::Button("Play VFX"))
				{
					if (vfx)
					{
						vfx->PlayVFX();
					}
				}
			}

			ImGui::End();
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([]()
		{
#ifndef _RETAIL
			CU::Vector2f resolution = Engine::GetInstance().GetResolution();
			ImGui::SetNextWindowPos({ 0.01f * resolution.x, 0.4f * resolution.y });
			ImGui::SetNextWindowContentSize({ 0.16f * resolution.x, 0.55f * resolution.y });
			bool open = true;
			ImGui::Begin("Lighting Settings", &open, ImGuiWindowFlags_NoSavedSettings);
			{
				if (ImGui::BeginTabBar("Lights"))
				{
					if (ImGui::BeginTabItem("Ambient Light"))
					{
						std::shared_ptr<GameObject> ambientLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("A_Light");
						bool active = ambientLight->GetActive();
						if (ImGui::Checkbox("Set Active##Ambient", &active)) ambientLight->SetActive(active);

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

						// Luminance
						ImGui::Text("Luminance");
						if (ImGui::BeginCombo("##LuminanceDropdown", GraphicsEngine::Get().LuminanceNames[static_cast<int>(GraphicsEngine::Get().LuminanceFunction)].c_str()))
						{
							for (unsigned i = 0; i < static_cast<unsigned>(Luminance::COUNT); i++)
							{
								if (ImGui::Selectable(GraphicsEngine::Get().LuminanceNames[i].c_str())) GraphicsEngine::Get().LuminanceFunction = static_cast<Luminance>(i);
							}
							ImGui::EndCombo();
						}

						// Bloom
						ImGui::Text("Bloom");
						if (ImGui::BeginCombo("##BloomDropdown", GraphicsEngine::Get().BloomNames[static_cast<int>(GraphicsEngine::Get().BloomFunction)].c_str()))
						{
							for (unsigned i = 0; i < static_cast<unsigned>(Bloom::COUNT); i++)
							{
								if (ImGui::Selectable(GraphicsEngine::Get().BloomNames[i].c_str())) GraphicsEngine::Get().BloomFunction = static_cast<Bloom>(i);
							}
							ImGui::EndCombo();
						}

						ImGui::SliderFloat("Bloom Intensity", &GraphicsEngine::Get().BloomStrength, 0, 1.0f);
						ImGui::SliderFloat("SSAO Noise Power", &GraphicsEngine::Get().SSAONoisePower, 0, 1.0f);
						ImGui::SliderFloat("SSAO Radius", &GraphicsEngine::Get().SSAORadius, 0, 0.5f);
						ImGui::SliderFloat("SSAO Bias", &GraphicsEngine::Get().SSAOBias, 0, 0.1f);
					}

					if (ImGui::BeginTabItem("Directional Light"))
					{
						std::shared_ptr<GameObject> directionalLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("D_Light");
						bool active = directionalLight->GetActive();
						if (ImGui::Checkbox("Set Active##Directional", &active)) directionalLight->SetActive(active);

						std::shared_ptr<DirectionalLight> dLight = directionalLight->GetComponent<DirectionalLight>();
						float intensity = dLight->GetIntensity();
						ImGui::Text("Intensity");
						if (ImGui::SliderFloat("##DirectionalIntensity", &intensity, 0, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) dLight->SetIntensity(intensity);
						ImGui::Spacing();
						float minBias = dLight->GetMinShadowBias();
						float maxBias = dLight->GetMaxShadowBias();
						float lightSize = dLight->GetLightSize();
						ImGui::Text("Shadow Min Bias");
						if (ImGui::SliderFloat("##DirectionalMinBias", &minBias, 0, 0.001f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
						ImGui::Text("Shadow Max Bias");
						if (ImGui::SliderFloat("##DirectionalMaxBias", &maxBias, 0, 0.1f, "%.5f")) dLight->SetShadowBias(minBias, maxBias);
						ImGui::Text("Light Size");
						if (ImGui::SliderFloat("##DirectionalSize", &lightSize, 0, 2000.0f)) dLight->SetLightSize(lightSize);
						ImGui::Spacing();
						float color[3] = { dLight->GetColor().x, dLight->GetColor().y, dLight->GetColor().z };
						ImGui::Text("Color");
						unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
						if (ImGui::ColorPicker3("##DirectionalColor", color, flags)) dLight->SetColor({ color[0], color[1], color[2] });
						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Pointlight"))
					{
						std::shared_ptr<GameObject> pointLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("P_Light");
						bool active = pointLight->GetActive();
						if (ImGui::Checkbox("Set Active##Point", &active))  pointLight->SetActive(active);

						std::shared_ptr<PointLight> pLight = pointLight->GetComponent<PointLight>();
						float value = pLight->GetIntensity();
						ImGui::Text("Intensity");
						if (ImGui::SliderFloat("##PointIntensity", &value, 0, 500000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) pLight->SetIntensity(value);

						ImGui::Spacing();

						float minBias = pLight->GetMinShadowBias();
						float maxBias = pLight->GetMaxShadowBias();
						float lightSize = pLight->GetLightSize();
						ImGui::Text("Shadow Min Bias");
						if (ImGui::SliderFloat("##PointMinBias", &minBias, 0, 0.001f, "%.5f")) pLight->SetShadowBias(minBias, maxBias);
						ImGui::Text("Shadow Max Bias");
						if (ImGui::SliderFloat("##PointMaxBias", &maxBias, 0, 0.01f, "%.5f")) pLight->SetShadowBias(minBias, maxBias);
						ImGui::Text("Light Size");
						if (ImGui::SliderFloat("##PointSize", &lightSize, 0, 10000.0f)) pLight->SetLightSize(lightSize);
						ImGui::Spacing();

						float color[3] = { pLight->GetColor().x, pLight->GetColor().y, pLight->GetColor().z };
						ImGui::Text("Color");
						unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
						if (ImGui::ColorPicker3("##PointColor", color, flags)) pLight->SetColor({ color[0], color[1], color[2] });

						ImGui::Spacing();

						float pos[3] = { pLight->GetPosition().x, pLight->GetPosition().y, pLight->GetPosition().z };
						ImGui::Text("Position");
						if (ImGui::DragFloat3("##PLightPos", pos)) pLight->gameObject->GetComponent<Transform>()->SetTranslation(pos[0], pos[1], pos[2]);
						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Spotlight"))
					{
						std::shared_ptr<GameObject> spotLight = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("S_Light");
						bool active = spotLight->GetActive();
						if (ImGui::Checkbox("Set Active##Spot", &active)) spotLight->SetActive(active);

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
						float lightSize = sLight->GetLightSize();
						ImGui::Text("Shadow Min Bias");
						if (ImGui::SliderFloat("##SpotMinBias", &minBias, 0, 0.001f, "%.5f")) sLight->SetShadowBias(minBias, maxBias);
						ImGui::Text("Shadow Max Bias");
						if (ImGui::SliderFloat("##SpotMaxBias", &maxBias, 0, 0.1f, "%.5f")) sLight->SetShadowBias(minBias, maxBias);
						ImGui::Text("Light Size");
						if (ImGui::SliderFloat("##SpotSize", &lightSize, 0, 10.0f)) sLight->SetLightSize(lightSize);
						ImGui::Spacing();

						float color[3] = { sLight->GetColor().x, sLight->GetColor().y, sLight->GetColor().z };
						ImGui::Text("Color");
						unsigned flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoSidePreview;
						if (ImGui::ColorPicker3("##SpotColor", color, flags)) sLight->SetColor({ color[0], color[1], color[2] });

						ImGui::Spacing();

						float pos[3] = { sLight->GetPosition().x, sLight->GetPosition().y, sLight->GetPosition().z };
						ImGui::Text("Position");
						if (ImGui::DragFloat3("##SLightPos", pos)) sLight->gameObject->GetComponent<Transform>()->SetTranslation(pos[0], pos[1], pos[2]);
						CU::Vector3f rotation = sLight->gameObject->GetComponent<Transform>()->GetRotation();
						float rot[3] = { rotation.x, rotation.y, rotation.z };
						ImGui::Text("Rotation");
						if (ImGui::DragFloat3("##SLightRot", rot)) sLight->gameObject->GetComponent<Transform>()->SetRotation(rot[0], rot[1], rot[2]);
						ImGui::EndTabItem();
							
					}

					ImGui::EndTabBar();
				}
				
				ImGui::End();
			}
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([]()
		{
#ifndef _RETAIL
			CU::Vector2f resolution = Engine::GetInstance().GetResolution();
			ImGui::SetNextWindowPos({ 0.18f * resolution.x, 0.02f * resolution.y });
			ImGui::SetNextWindowContentSize({ 0.16f * resolution.x, 0.2f * resolution.y });
			bool open = true;
			ImGui::Begin("Performance Info", &open, ImGuiWindowFlags_NoSavedSettings);
			if (ImGui::BeginTable("PerformanceTable", 2, 0, { 0.18f * resolution.x, 0 }))
			{
				ImGuiStyle& style = ImGui::GetStyle();
				style.CellPadding = { 0.001f * resolution.x, 0.004f * resolution.y };

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
			}
			ImGui::End();
#endif
		});

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([]()
		{
#ifndef _RETAIL
			CU::Vector2f resolution = Engine::GetInstance().GetResolution();
			ImGui::SetNextWindowPos({ 0.85f * resolution.x, 0.02f * resolution.y });
			ImGui::SetNextWindowContentSize({ 0.24f * resolution.x, 0.26f * resolution.y });
			bool open = true;
			ImGui::Begin("Controller Info", &open, ImGuiWindowFlags_NoSavedSettings);

			InputHandler& cont = Engine::GetInstance().GetInputHandler();

			ImGui::Text("Left Stick: ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("LeftStick").x);
			ImGui::SameLine();
			ImGui::Text(", ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("LeftStick").y);

			ImGui::Text("Right Stick: ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("RightStick").x);
			ImGui::SameLine();
			ImGui::Text(", ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("RightStick").y);

			ImGui::Text("Left Trigger: ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction("LeftTrigger"));

			ImGui::Text("Right Trigger: ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction("RightTrigger"));


			ImVec4 red = { 1.0f, 0, 0, 1.0f };
			ImVec4 green = { 0, 1.0f, 0, 1.0f };
			ImVec4 color = (cont.GetBinaryAction("A_Gamepad") ? green : red);
			ImGui::TextColored(color, "A");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("B") ? green : red);
			ImGui::TextColored(color, "B");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("X") ? green : red);
			ImGui::TextColored(color, "X");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("Y") ? green : red);
			ImGui::TextColored(color, "Y");

			color = (cont.GetBinaryAction("LB") ? green : red);
			ImGui::TextColored(color, "LB");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("RB") ? green : red);
			ImGui::TextColored(color, "RB");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("LS") ? green : red);
			ImGui::TextColored(color, "LS");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("RS") ? green : red);
			ImGui::TextColored(color, "RS");

			color = (cont.GetBinaryAction("Left") ? green : red);
			ImGui::TextColored(color, "Left");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("Up") ? green : red);
			ImGui::TextColored(color, "Up");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("Right") ? green : red);
			ImGui::TextColored(color, "Right");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("Down") ? green : red);
			ImGui::TextColored(color, "Down");

			color = (cont.GetBinaryAction("Start") ? green : red);
			ImGui::TextColored(color, "Start");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("Back") ? green : red);
			ImGui::TextColored(color, "Back");

			ImGui::Separator();

			ImGui::Text("Mouse Pos: ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("MousePos").x);
			ImGui::SameLine();
			ImGui::Text(", ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("MousePos").y);

			ImGui::Text("NDC Mouse Pos: ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseNDCPos").x);
			ImGui::SameLine();
			ImGui::Text(", ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseNDCPos").y);

			ImGui::Text("Mouse Delta: ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseDelta").x);
			ImGui::SameLine();
			ImGui::Text(", ");
			ImGui::SameLine();
			ImGui::Text("%.2f", cont.GetAnalogAction2D("MouseDelta").y);

			color = (cont.GetBinaryAction("W") ? green : red);
			ImGui::TextColored(color, "W");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("A") ? green : red);
			ImGui::TextColored(color, "A");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("S") ? green : red);
			ImGui::TextColored(color, "S");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("D") ? green : red);
			ImGui::TextColored(color, "D");

			color = (cont.GetBinaryAction("LMB") ? green : red);
			ImGui::TextColored(color, "LMB");
			ImGui::SameLine();
			color = (cont.GetBinaryAction("RMB") ? green : red);
			ImGui::TextColored(color, "RMB");

			color = (cont.GetBinaryAction("SharedAction") ? green : red);
			ImGui::TextColored(color, "SharedAction");

			ImGui::End();
#endif
		});

	/*Engine::GetInstance().GetImGuiHandler().AddNewFunction([]()
		{
#ifndef _RETAIL
			CU::Vector2f resolution = Engine::GetInstance().GetResolution();
			ImGui::SetNextWindowPos({ 0.85f * resolution.x, 0.32f * resolution.y });
			ImGui::SetNextWindowContentSize({ 0.24f * resolution.x, 0.24f * resolution.y });
			bool open = true;
			ImGui::Begin("Resolution", &open, ImGuiWindowFlags_NoSavedSettings);

			if (ImGui::Button("Set Maximized"))
			{
				GraphicsEngine::Get().MaximizeWindowSize();
			}

			if (ImGui::Button("1280 x 720"))
			{
				GraphicsEngine::Get().SetResolution(1280.0f, 720.0f);
				Engine::GetInstance().SetResolution(1280.0f, 720.0f);
			}
			if (ImGui::Button("1600 x 900"))
			{
				GraphicsEngine::Get().SetResolution(1600.0f, 900.0f);
				Engine::GetInstance().SetResolution(1600.0f, 900.0f);
			}
			if (ImGui::Button("1920 x 1080"))
			{
				GraphicsEngine::Get().SetResolution(1920.0f, 1080.0f);
				Engine::GetInstance().SetResolution(1920.0f, 1080.0f);
			}
			if (ImGui::Button("2560 x 1440"))
			{
				GraphicsEngine::Get().SetResolution(2560.0f, 1440.0f);
				Engine::GetInstance().SetResolution(2560.0f, 1440.0f);
			}
			if (ImGui::Button("3840 x 2160"))
			{
				GraphicsEngine::Get().SetResolution(3840.0f, 2160.0f);
				Engine::GetInstance().SetResolution(3840.0f, 2160.0f);
			}

			ImGui::End();
#endif
		});*/

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([]()
		{
#ifndef _RETAIL
			CU::Vector2f resolution = Engine::GetInstance().GetResolution();
			ImGui::SetNextWindowPos({ 0.85f * resolution.x, 0.32f * resolution.y });
			ImGui::SetNextWindowContentSize({ 0.24f * resolution.x, 0.24f * resolution.y });
			bool open = true;
			ImGui::Begin("Particle Position", &open, ImGuiWindowFlags_NoSavedSettings);

			std::shared_ptr<GameObject> psObject = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("ParticleTest");
			std::shared_ptr<Transform> psTransform = psObject->GetComponent<Transform>();
			CU::Vector3f psPos = psTransform->GetTranslation(true);
			float pos[3] = { psPos.x, psPos.y, psPos.z };
			if (ImGui::DragFloat3("Particle System Position", pos))
			{
				psTransform->SetTranslation(pos[0], pos[1], pos[2]);
			}

			ImGui::End();
#endif
		});
}

void FeatureShowcase::UpdateApplication()
{
	if (Engine::GetInstance().GetInputHandler().GetBinaryAction("SPACE") && !Engine::GetInstance().GetInputHandler().GetBinaryAction("RMB"))
	{
		currentAnimation += 1;
		if (currentAnimation >= animationNames.size())
		{
			currentAnimation = 0;
		}

		std::shared_ptr<AnimatedModel> tgaBroModel = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("TgaBro")->GetComponent<AnimatedModel>();
		tgaBroModel->SetCurrentAnimationOnLayer(animationNames[currentAnimation], "", 0.5f);
	}

	if (Engine::GetInstance().GetInputHandler().GetBinaryAction("F6"))
	{
		currentDebugMode += 1;
		if (currentDebugMode >= static_cast<unsigned>(DebugMode::COUNT))
		{
			currentDebugMode = 0;
		}

		GraphicsEngine::Get().CurrentDebugMode = static_cast<DebugMode>(currentDebugMode);
	}

	if (Engine::GetInstance().GetInputHandler().GetBinaryAction("F7"))
	{
		GraphicsEngine::Get().SSAOEnabled = !GraphicsEngine::Get().SSAOEnabled;
	}
}
