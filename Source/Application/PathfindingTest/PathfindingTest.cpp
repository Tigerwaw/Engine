#include "PathfindingTest.h"
#include <GameEngine/Engine.h>

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new PathfindingTest();
}

void PathfindingTest::InitializeApplication()
{
	Engine::GetInstance().GetInputHandler().RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Clicked);

	Engine::GetInstance().GetDebugDrawer().InitializeDebugDrawer();
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_TestScene.json");
}
