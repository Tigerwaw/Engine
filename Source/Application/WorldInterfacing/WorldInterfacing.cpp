#include "WorldInterfacing.h"
#include <GameEngine/Engine.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "AI/Components/ControllerMove.h"

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new WorldInterfacing();
}

void WorldInterfacing::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_WorldInterfacing.json");

	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Player")->AddComponent<ControllerMove>(150.0f, ControllerMove::ControllerType::Player);
	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Robot1")->AddComponent<ControllerMove>(100.0f, ControllerMove::ControllerType::AIPolling);
	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Robot2")->AddComponent<ControllerMove>(100.0f, ControllerMove::ControllerType::AIPolling);
	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Robot3")->AddComponent<ControllerMove>(100.0f, ControllerMove::ControllerType::AIEvents);
	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Robot4")->AddComponent<ControllerMove>(100.0f, ControllerMove::ControllerType::AIEvents);
}
