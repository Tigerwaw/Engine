#include "GameEngine/Application.h"
#include "GameEngine/EntryPoint.h"

#include <GameEngine/Engine.h>

class APPNAME : public Application
{
public:
	APPNAME() {}
    ~APPNAME() {}

    void InitializeApplication() override;
};

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new APPNAME();
}

void APPNAME::InitializeApplication()
{

}
