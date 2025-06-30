#include "Enginepch.h"
#include "NetworkClient.h"
#include <GameEngine/Application/AppSettings.h>
#include <GameEngine/Engine.h>
#include <GameEngine/Time/Timer.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"

#include "Controller.h"
#include <Utilities/CommonUtilities/Random.hpp>

inline const std::array<const char*, 10> clientNames = { "Lars", "Ingvar", "Jonas", "Katarina", "Sara", "Klas", "Vera", "Tor", "Freja", "Wilmer" };

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new NetworkClient();
}

void NetworkClient::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	GraphicsEngine::Get().DrawColliders = false;
	Engine::Get().GetSceneHandler().LoadScene("Scenes/SC_NetworkingScene.json");
	myClient.SetUsername(clientNames[Utilities::RandomInRange(0, 9)]);
	myClient.ConnectClient("");

	Engine::Get().GetInputHandler().RegisterBinaryAction("ToggleLerp", Keys::SPACE, GenericInput::ActionType::Clicked);

	Engine::Get().GetImGuiHandler().AddNewFunction([this]() 
	{
		if (ImGui::Begin("Network Stats"))
		{
			ImGui::Text("FPS: %i", Engine::Get().GetTimer().GetFPS());
			ImGui::Text("Received Data: %i bytes/s", myClient.GetReceivedData());
			ImGui::Text("Sent Data: %i bytes/s", myClient.GetSentData());
			ImGui::Text("Guaranteed Messages Sent: %i", myClient.GetNrOfGuaranteedMessagesSent());
			ImGui::Text("Guaranteed Messages Lost: %i", myClient.GetNrOfGuaranteedMessagesLost());
			ImGui::Text("Estimated Package Loss: %.3f%%", myClient.GetEstimatedPackageLoss());

			ImGui::Text("Ping: %.0f ms", myClient.GetRTT() * 1000.0f);

			ImGui::Spacing();
			bool toggleLerp = myClient.GetIsLerpingPositions();
			ImGui::Checkbox("Toggle Object Position Lerp", &toggleLerp);
			myClient.ToggleLerpPositions(toggleLerp);

			ImGui::Spacing();
			if (ImGui::BeginChild("GuaranteedMessages"))
			{
				//if (ImGui::BeginTable("GM_Table", 3))
				//{
				//	for (auto& [id, data] : myClient.GetGuaranteedMessageData())
				//	{
				//		ImGui::TableSetColumnIndex(0);
				//		ImGui::TableHeader("GMID");
				//		ImGui::Text("%i", id);
				//		ImGui::TableSetColumnIndex(1);
				//		ImGui::TableHeader("Attempts");
				//		ImGui::Text("%i", data.myAttempts);
				//		ImGui::TableSetColumnIndex(2);
				//		ImGui::TableHeader("Message Type");
				//		ImGui::Text("%i", data.myGuaranteedMessageBuffer.GetBuffer()[0]);
				//		ImGui::TableNextRow();
				//	}

				//}
				//ImGui::EndTable();
			}
			ImGui::EndChild();
		}
		ImGui::End();
	});
}

void NetworkClient::UpdateApplication()
{
	myClient.Update();
}
