#include "Enginepch.h"
#include "Movement1.h"
#include <GameEngine/Application/AppSettings.h>
#include <GameEngine/Engine.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "AI/Components/ControllerMove.h"
#include "AI/PollingStation.h"
#include "AI/Components/WrapAroundWorld.h"

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new Movement1();
}

void Movement1::InitializeApplication()
{
	Engine::Get().RecalculateShadowFrustum = false;
	Engine::Get().GetSceneHandler().LoadScene("Movement.json");

	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->AddComponent<Transform>();
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("TgaBroBlue.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		go->AddComponent<WrapAroundWorld>();
		go->AddComponent<ControllerMove>(150.0f, 15.0f, ControllerMove::ControllerType::Wander);
		PollingStation::Get().SetWanderer(go);
		PollingStation::Get().AddWatchedActor(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}

	for (int i = 0; i < 4; i++)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->AddComponent<Transform>(Math::Vector3f(static_cast<float>(std::rand() % 1000 - std::rand() % 1000), 0, static_cast<float>(std::rand() % 1000 - std::rand() % 1000)));
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("TgaBroRed.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		go->AddComponent<WrapAroundWorld>();
		go->AddComponent<ControllerMove>(80.0f, 5.0f, ControllerMove::ControllerType::Seek);

		PollingStation::Get().AddWatchedActor(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}

	for (int i = 0; i < 10; i++)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->AddComponent<Transform>(Math::Vector3f(static_cast<float>(std::rand() % 1000 - std::rand() % 1000), 0, static_cast<float>(std::rand() % 1000 - std::rand() % 1000)));
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("TgaBroGreen.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		go->AddComponent<WrapAroundWorld>();
		go->AddComponent<ControllerMove>(80.0f, 5.0f, ControllerMove::ControllerType::Separate);

		PollingStation::Get().AddWatchedActor(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}
}

void Movement1::UpdateApplication()
{
	PollingStation::Get().Update();
}
