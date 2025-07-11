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
	Engine::Get().RecalculateShadowFrustum = false;
	Engine::Get().DrawColliders = true;
	Engine::Get().GetSceneHandler().LoadScene("Scenes/SC_NetworkingScene.json");
	myServer.StartServer();

	Engine::Get().GetImGuiHandler().AddNewFunction([this]()
		{
			if (ImGui::Begin("Network Stats"))
			{
				ImGui::Text("FPS: %i", Engine::Get().GetTimer().GetFPS());
				ImGui::Text("Received Data: %i bytes/s", myServer.GetReceivedData());
				ImGui::Text("Sent Data: %i bytes/s", myServer.GetSentData());
				ImGui::Text("Guaranteed Messages Sent: %i", myServer.GetNrOfGuaranteedMessagesSent());
				ImGui::Text("Guaranteed Messages Lost: %i", myServer.GetNrOfGuaranteedMessagesLost());
				ImGui::Text("Estimated Package Loss: %.3f%%", myServer.GetEstimatedPackageLoss());

				ImGui::Spacing();
				if (ImGui::BeginChild("Clients"))
				{
					for (size_t clientIndex = 0; clientIndex < myServer.GetClients().size(); clientIndex++)
					{
						auto& client = myServer.GetClients()[clientIndex];
						ImGui::Text("Client ID: %s, Ping: %.0f ms", client.myUsername.c_str(), client.myRTT * 1000.0f);
					}
				}
				ImGui::EndChild();

				ImGui::Spacing();
				if (ImGui::BeginChild("GuaranteedMessages"))
				{
					//if (ImGui::BeginTable("GM_Table", 4))
					//{
					//	for (auto& [id, data] : myServer.GetGuaranteedMessageData())
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
					//		ImGui::TableSetColumnIndex(3);
					//		ImGui::TableHeader("Client Address");
					//		ImGui::Text("%i", data.myRecipientAddress.sin_addr.S_un.S_addr);
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

void NetworkServer::UpdateApplication()
{
	myServer.Update();
}
