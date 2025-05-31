#include "Enginepch.h"
#include "Movement2.h"
#include <GameEngine/Application/AppSettings.h>
#include <GameEngine/Engine.h>

#include <GameEngine/ComponentSystem/GameObject.h>
#include <GameEngine/ComponentSystem/Components/Transform.h>
#include <GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h>
#include "AI/Components/ControllerMoveWeighted.h"
#include "AI/PollingStation.h"
#include "AI/Components/WrapAroundWorld.h"

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new Movement2();
}

void Movement2::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::Get().GetSceneHandler().LoadScene("Scenes/SC_Movement.json");

	for (int i = 0; i < 20; i++)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->AddComponent<Transform>(Math::Vector3f(static_cast<float>(std::rand() % 800 - std::rand() % 800), 0, static_cast<float>(std::rand() % 500 - std::rand() % 500)));
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_TgaBroBlue.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		auto cont = go->AddComponent<ControllerMoveWeighted>(300.0f, 30.0f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::Separate, 8.0f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::Cohesion, 5.0f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::VelocityMatch, 3.0f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::CollisionAvoidance, 5.0f);

		PollingStation::Get().AddWatchedActor(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}
}

void Movement2::UpdateApplication()
{
	PollingStation::Get().Update();
}
