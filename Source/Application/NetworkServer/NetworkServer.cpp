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

	Engine::Get().GetImGuiHandler().AddNewFunction([this]()
		{
			if (ImGui::Begin("Network Stats"))
			{
				ImGui::Text("Received Data: %i bytes/s", myServer.GetReceivedData());
				ImGui::Text("Sent Data: %i bytes/s", myServer.GetSentData());

				ImGui::End();
			}
		});
}

void NetworkServer::UpdateApplication()
{
	myServer.Update();
}
