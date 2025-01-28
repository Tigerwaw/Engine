#include "Enginepch.h"
#include "NetworkClient.h"
#include <GameEngine/Engine.h>

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new NetworkClient();
}

void NetworkClient::InitializeApplication()
{
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_BaseScene.json");

	Engine::GetInstance().GetImGuiHandler().AddNewFunction([this] 
		{
			CU::Vector2f windowSize = Engine::GetInstance().GetWindowSize();
			ImGui::SetNextWindowPos({ 0.01f * windowSize.x, 0.02f * windowSize.y });
			ImGui::SetNextWindowContentSize({ 0.16f * windowSize.x, 0.35f * windowSize.y });
			if (ImGui::Begin("Chat window"))
			{
				std::string input;
				if (ImGui::InputText("##input", &input, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					myClient.SendInput(input);
				}

				ImGui::End();
			}
		});
}

void NetworkClient::UpdateApplication()
{
}
