#include "PathfindingTest.h"
#include <GameEngine/Engine.h>
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"

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

void PathfindingTest::UpdateApplication()
{
	Engine& engine = Engine::GetInstance();

	if (engine.GetInputHandler().GetBinaryAction("LMB"))
	{
		std::shared_ptr<Transform> camTransform = engine.GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Transform>();
		CU::Vector3f origin = camTransform->GetTranslation(true);
		CU::Vector3f direction = camTransform->GetForwardVector(true);
		CU::Vector3f hitPoint;
		Engine::GetInstance().GetDebugDrawer().DrawLine(origin, origin + direction * 1000.0f, CU::Vector4f(1.0f, 0, 0, 1.0f));
		if (engine.GetSceneHandler().Raycast(origin, direction, hitPoint))
		{
			std::string log = "Hit object at " + std::to_string(hitPoint.x) + ", " + std::to_string(hitPoint.y) + ", " + std::to_string(hitPoint.z);
			LOG(LogSceneHandler, Log, log.c_str());
		}
	}
}
