#include "Enginepch.h"
#include "DecisionMaking.h"
#include <GameEngine/Application/AppSettings.h>
#include <GameEngine/Engine.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "Math/Curve.h"
#include "AI/PollingStation.h"
#include "AI/Components/StateMachineController.h"
#include "AI/Components/DecisionTreeController.h"
#include "AI/Components/BehaviourTreeController.h"
#include "HealthComponent.h"

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new DecisionMaking();
}

void DecisionMaking::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::Get().GetSceneHandler().LoadScene("Scenes/SC_BaseScene.json");

	for (int x = -2; x < 3; x++)
	{
		for (int y = -2; y < 3; y++)
		{
			std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
			auto transform = go->AddComponent<Transform>(Math::Vector3f(500.0f * x, 0, 500.0f * y));
			transform->SetUniformScale(100.0f);
			auto model = go->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("SM_Sphere.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("MAT_MatballOne.json")->material);

			PollingStation::Get().AddWall(go);
			Engine::Get().GetSceneHandler().Instantiate(go);
		}
	}

	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->SetName("SMCont");
		go->AddComponent<Transform>(Math::Vector3f(300.0f, 0, -500.0f));
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorGreen.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		go->AddComponent<HealthComponent>();

		{
			auto ps = go->AddComponent<ParticleSystem>();
			ParticleEmitterSettings psSettings;
			psSettings.EmitterSize = { 100.0f, 100.0f, 100.0f };
			psSettings.Lifetime = 1.0f;
			psSettings.SpawnRate = 100.0f;
			Math::Curve<Math::Vector2f> sizeCurve;
			sizeCurve.AddKey(0.0f, { 1.0f, 1.0f });
			sizeCurve.AddKey(1.0f, { 80.0f, 80.0f });
			psSettings.Size = sizeCurve;
			psSettings.VelocityRangeMin = { -50.0, 100.0, -50.0 };
			psSettings.VelocityRangeMax = { 50.0, 300.0, 50.0 };
			psSettings.GravityScale = 10.0f;
			Math::Curve<Math::Vector4f> colorCurve;
			colorCurve.AddKey(0.0f, { 1.0, 1.0, 1.0, 0.0 });
			colorCurve.AddKey(0.5f, { 1.0, 1.0, 1.0, 0.75 });
			colorCurve.AddKey(1.0f, { 1.0, 1.0, 1.0, 0.0 });
			psSettings.Color = colorCurve;
			auto& pe = ps->AddEmitter(psSettings);
			pe.SetMaterial(AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ExplosionParticle.json")->material);
			ps->SetActive(false);
		}

		go->AddComponent<StateMachineController>();

		PollingStation::Get().AddAIActor(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}

	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->SetName("DTCont");
		go->AddComponent<Transform>(Math::Vector3f(-200.0f, 0, 500.0f), Math::Vector3f(0, 180.0f, 0));
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorGreen.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		go->AddComponent<HealthComponent>();

		{
			auto ps = go->AddComponent<ParticleSystem>();
			ParticleEmitterSettings psSettings;
			psSettings.EmitterSize = { 100.0f, 100.0f, 100.0f };
			psSettings.Lifetime = 1.0f;
			psSettings.SpawnRate = 100.0f;
			Math::Curve<Math::Vector2f> sizeCurve;
			sizeCurve.AddKey(0.0f, { 1.0f, 1.0f });
			sizeCurve.AddKey(1.0f, { 80.0f, 80.0f });
			psSettings.Size = sizeCurve;
			psSettings.VelocityRangeMin = { -50.0, 100.0, -50.0 };
			psSettings.VelocityRangeMax = { 50.0, 300.0, 50.0 };
			psSettings.GravityScale = 10.0f;
			Math::Curve<Math::Vector4f> colorCurve;
			colorCurve.AddKey(0.0f, { 1.0, 1.0, 1.0, 0.0 });
			colorCurve.AddKey(0.5f, { 1.0, 1.0, 1.0, 0.75 });
			colorCurve.AddKey(1.0f, { 1.0, 1.0, 1.0, 0.0 });
			psSettings.Color = colorCurve;
			auto& pe = ps->AddEmitter(psSettings);
			pe.SetMaterial(AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ExplosionParticle.json")->material);
			ps->SetActive(false);
		}

		go->AddComponent<DecisionTreeController>();

		PollingStation::Get().AddAIActor(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}
	
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->SetName("BTCont");
		go->AddComponent<Transform>(Math::Vector3f(400.0f, 0, 300.0f), Math::Vector3f(0, -90.0f, 0));
		auto model = go->AddComponent<AnimatedModel>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SK_C_TGA_Bro.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ColorGreen.json")->material);
		model->AddAnimationToLayer("Idle", AssetManager::Get().GetAsset<AnimationAsset>("Animations/TgaBro/Idle/A_C_TGA_Bro_Idle_Breathing.fbx")->animation, "", true);
		go->AddComponent<HealthComponent>();

		{
			auto ps = go->AddComponent<ParticleSystem>();
			ParticleEmitterSettings psSettings;
			psSettings.EmitterSize = { 100.0f, 100.0f, 100.0f };
			psSettings.Lifetime = 1.0f;
			psSettings.SpawnRate = 100.0f;
			Math::Curve<Math::Vector2f> sizeCurve;
			sizeCurve.AddKey(0.0f, { 1.0f, 1.0f });
			sizeCurve.AddKey(1.0f, { 80.0f, 80.0f });
			psSettings.Size = sizeCurve;
			psSettings.VelocityRangeMin = { -50.0, 100.0, -50.0 };
			psSettings.VelocityRangeMax = { 50.0, 300.0, 50.0 };
			psSettings.GravityScale = 10.0f;
			Math::Curve<Math::Vector4f> colorCurve;
			colorCurve.AddKey(0.0f, { 1.0, 1.0, 1.0, 0.0 });
			colorCurve.AddKey(0.5f, { 1.0, 1.0, 1.0, 0.75 });
			colorCurve.AddKey(1.0f, { 1.0, 1.0, 1.0, 0.0 });
			psSettings.Color = colorCurve;
			auto& pe = ps->AddEmitter(psSettings);
			pe.SetMaterial(AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_ExplosionParticle.json")->material);
			ps->SetActive(false);
		}

		go->AddComponent<BehaviourTreeController>();

		PollingStation::Get().AddAIActor(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}

	auto smCont = Engine::Get().GetSceneHandler().FindGameObjectByName("SMCont");
	auto dtCont = Engine::Get().GetSceneHandler().FindGameObjectByName("DTCont");
	auto btCont = Engine::Get().GetSceneHandler().FindGameObjectByName("BTCont");
	smCont->GetComponent<StateMachineController>()->SetTarget(dtCont);
	dtCont->GetComponent<DecisionTreeController>()->SetTarget(btCont);
	btCont->GetComponent<BehaviourTreeController>()->SetTarget(smCont);

	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->SetName("HWell");
		auto transform = go->AddComponent<Transform>(Math::Vector3f(-800.0f, 0, 0.0f));
		transform->SetRotation(0, 90.0f, 0);
		go->AddComponent<Model>(AssetManager::Get().GetAsset<MeshAsset>("Assets/SM_Chest.fbx")->mesh, AssetManager::Get().GetAsset<MaterialAsset>("Materials/MAT_Chest.json")->material);

		PollingStation::Get().SetHealingWell(go);
		Engine::Get().GetSceneHandler().Instantiate(go);
	}
}

void DecisionMaking::UpdateApplication()
{
}
