#include "Enginepch.h"
#include "NetworkClient.h"
#include <GameEngine/Engine.h>

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"

#include "Controller.h"

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new NetworkClient();
}

void NetworkClient::InitializeApplication()
{
	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	GraphicsEngine::Get().DrawColliders = true;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_NetworkingScene.json");
}

void NetworkClient::UpdateApplication()
{
	myClient.Update();
}
