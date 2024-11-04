#include "WorldInterfacing.h"
#include <GameEngine/Engine.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "AI/Components/ControllerMove.h"
#include "AI/PollingStation.h"

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new WorldInterfacing();
}

void WorldInterfacing::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_WorldInterfacing.json");

	auto wanderer = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Wanderer");
	wanderer->AddComponent<ControllerMove>(150.0f, ControllerMove::ControllerType::Wander);
	PollingStation::Get().SetWanderer(wanderer);

	std::vector<std::string> seekerNames = { "Seeker1", "Seeker2", "Seeker3", "Seeker4" };
	for (auto& seekerName : seekerNames)
	{
		auto seeker = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(seekerName);
		seeker->AddComponent<ControllerMove>(100.0f, ControllerMove::ControllerType::Seek);
		PollingStation::Get().AddWatchedActor(seeker);
	}

	std::vector<std::string> separatorNames = { "Separator1", "Separator2", "Separator3", "Separator4", "Separator5", 
											"Separator6", "Separator7", "Separator8", "Separator9", "Separator10" };
	for (auto& separatorName : separatorNames)
	{
		auto separator = Engine::GetInstance().GetSceneHandler().FindGameObjectByName(separatorName);
		separator->AddComponent<ControllerMove>(100.0f, ControllerMove::ControllerType::Separate);
		PollingStation::Get().AddWatchedActor(separator);
	}
}

void WorldInterfacing::UpdateApplication()
{
	PollingStation::Get().Update();
}
