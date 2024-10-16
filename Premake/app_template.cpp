#include "APPNAME.h"
#include <GameEngine/Engine.h>

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new APPNAME();
}

void APPNAME::InitializeApplication()
{

}

void APPNAME::UpdateApplication()
{

}
