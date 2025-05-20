#include "Enginepch.h"
#include "NetworkServer.h"
#include <GameEngine/Engine.h>
#include <GameEngine/Time/Timer.h>

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new NetworkServer();
}

void NetworkServer::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	GraphicsEngine::Get().DrawColliders = true;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_NetworkingScene.json");
	myServer.StartServer();
}

void NetworkServer::UpdateApplication()
{
	myServer.Update();
}
