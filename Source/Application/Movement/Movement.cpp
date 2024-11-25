#include "Movement.h"
#include <GameEngine/Engine.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "AI/Components/ControllerMoveWeighted.h"
#include "AI/PollingStation.h"
#include "AI/Components/WrapAroundWorld.h"

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new Movement();
}

void Movement::InitializeApplication()
{
	//GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_Movement.json");

	for (int i = 0; i < 50; i++)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->AddComponent<Transform>();
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_TgaBroBlue.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		go->AddComponent<WrapAroundWorld>();
		auto cont = go->AddComponent<ControllerMoveWeighted>(200.0f, 5.0f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::Separate, 0.7f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::Cohesion, 0.2f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::VelocityMatch, 0.05f);
		cont->AddControllerType(ControllerMoveWeighted::ControllerType::Wander, 0.05f);

		myActors.emplace_back(go);
		PollingStation::Get().AddWatchedActor(go);
		Engine::GetInstance().GetSceneHandler().Instantiate(go);
	}

	for (int i = 1; i < myActors.size(); i++)
	{
		myActors[i]->GetComponent<ControllerMoveWeighted>()->SetTarget(myActors[i - 1]);
	}
}

void Movement::UpdateApplication()
{
	PollingStation::Get().Update();
}
