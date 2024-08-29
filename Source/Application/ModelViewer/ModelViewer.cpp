#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"

class ModelViewer : public Application
{
public:
	ModelViewer() {}
    ~ModelViewer() {}

    void InitializeApplication() override;
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

	Engine::GetInstance().GetWindowsEventHandler().AddEvent(WindowsEventHandler::WindowsEvent::DropFiles, [](MSG aMSG)
		{
			TCHAR name[MAX_PATH];

			HDROP hDrop = (HDROP)aMSG.wParam;

			POINT mousePos;
			DragQueryPoint(hDrop, &mousePos);

			DragQueryFile(hDrop, 0, name, MAX_PATH);
			std::filesystem::path filePath = name;

			DragFinish(hDrop);

			AssetManager::Get().RegisterAsset(filePath);
			std::filesystem::path assetPath = AssetManager::Get().MakeRelative(filePath);
			if (assetPath == "")
			{
				LOG(LogApplication, Error, "Couldn't find asset path");
				return;
			}

			std::shared_ptr<GameObject> go = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Model");
			if (!go)
			{
				LOG(LogApplication, Error, "Couldn't find game object");
			}

			const std::string assetName = assetPath.filename().stem().string();
			if (assetName.starts_with("SM"))
			{
				go->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>(assetPath)->mesh, AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
			}
			else if (assetName.starts_with("SK"))
			{
				go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>(assetPath)->mesh, AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material);
				go->GetComponent<AnimatedModel>()->StopAnimation();
			}
			else if (assetName.starts_with("A"))
			{
				std::shared_ptr<AnimatedModel> animModel = go->GetComponent<AnimatedModel>();
				if (!animModel)
				{
					LOG(LogApplication, Warning, "The currently loaded model does not support animations!");
					return;
				}

				animModel->AddAnimationToLayer(assetName, AssetManager::Get().GetAsset<AnimationAsset>(assetPath)->animation, "", true);
				animModel->SetCurrentAnimationOnLayer(assetName, "", 0, 0);
				animModel->PlayAnimation();
			}
			else if (assetName.starts_with("MAT"))
			{
				std::shared_ptr<Model> model = go->GetComponent<Model>();
				if (!model)
				{
					LOG(LogApplication, Warning, "No model exists to apply material to!");
					return;
				}

				model->SetMaterialOnSlot(0, AssetManager::Get().GetAsset<MaterialAsset>(assetPath)->material);
			}
			else if (assetName.starts_with("T"))
			{
				std::shared_ptr<Model> model = go->GetComponent<Model>();
				std::shared_ptr<AnimatedModel> animModel = go->GetComponent<AnimatedModel>();
				if (model)
				{
					if (model->GetMaterialOnSlot(0) == AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->material)
					{
						std::shared_ptr<Material> newMat = std::make_shared<Material>();
						newMat->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>("Default_C")->texture);
						newMat->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>("Default_N")->texture);
						newMat->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>("Default_M")->texture);
						model->SetMaterialOnSlot(0, newMat);
					}

					std::shared_ptr<Material> modelMat = model->GetMaterialOnSlot(0);
					
					if (assetName.ends_with("C"))
					{
						modelMat->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>(assetPath)->texture);
					}
					else if (assetName.ends_with("N"))
					{
						modelMat->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>(assetPath)->texture);
					}
					else if (assetName.ends_with("M"))
					{
						modelMat->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>(assetPath)->texture);
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
						modelMat->SetAlbedoTexture(AssetManager::Get().GetAsset<TextureAsset>(assetPath)->texture);
					}
					else if (assetName.ends_with("N"))
					{
						modelMat->SetNormalTexture(AssetManager::Get().GetAsset<TextureAsset>(assetPath)->texture);
					}
					else if (assetName.ends_with("M"))
					{
						modelMat->SetMaterialTexture(AssetManager::Get().GetAsset<TextureAsset>(assetPath)->texture);
					}
				}
				else
				{
					LOG(LogApplication, Warning, "No model exists to apply texture to!");
				}
			}
			else
			{
				LOG(LogApplication, Error, "Can't load filetype {}", assetPath.string().c_str());
			}
		});
}
