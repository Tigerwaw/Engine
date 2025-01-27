#include "Enginepch.h"
#include "NetworkServer.h"
#include <GameEngine/Engine.h>

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new NetworkServer();
}

void NetworkServer::InitializeApplication()
{
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_BaseScene.json");
}

void NetworkServer::UpdateApplication()
{
}
