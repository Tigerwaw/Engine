#include "Enginepch.h"
#include "NetworkServer.h"
#include <GameEngine/Application/AppSettings.h>
#include <GameEngine/Engine.h>
#include <GameEngine/Time/Timer.h>

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new NetworkServer();
}

void NetworkServer::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	GraphicsEngine::Get().DrawColliders = true;
	Engine::Get().GetSceneHandler().LoadScene("Scenes/SC_NetworkingScene.json");
	myServer.StartServer();
}

void NetworkServer::UpdateApplication()
{
	myServer.Update();
}
