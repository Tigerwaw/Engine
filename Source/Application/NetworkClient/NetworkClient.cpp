#include "Enginepch.h"
#include "NetworkClient.h"
#include <GameEngine/Application/AppSettings.h>
#include <GameEngine/Engine.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"

#include "Controller.h"

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new NetworkClient();
}

void NetworkClient::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	GraphicsEngine::Get().DrawColliders = true;
	Engine::Get().GetSceneHandler().LoadScene("Scenes/SC_NetworkingScene.json");
	myClient.ConnectClient("");

	Engine::Get().GetInputHandler().RegisterBinaryAction("ToggleLerp", Keys::SPACE, GenericInput::ActionType::Clicked);

	Engine::Get().GetImGuiHandler().AddNewFunction([this]() 
	{
		if (ImGui::Begin("Network Stats"))
		{
			ImGui::Text("Received Data: %i bytes/s", myClient.GetReceivedData());
			ImGui::Text("Sent Data: %i bytes/s", myClient.GetSentData());
			ImGui::Text("Guaranteed Messages Sent: %i", myClient.GetNrOfGuaranteedMessagesSent());
			ImGui::Text("Guaranteed Messages Lost: %i", myClient.GetNrOfGuaranteedMessagesLost());
			ImGui::Text("Estimated Package Loss: %s%%", std::format("{:.3f}", myClient.GetEstimatedPackageLoss()).c_str());

			ImGui::Text("Ping: %s", std::format("{:.2f}", myClient.GetRTT() * 1000.0f).c_str());

			ImGui::End();
		}
	});
}

void NetworkClient::UpdateApplication()
{
	myClient.Update();
}
