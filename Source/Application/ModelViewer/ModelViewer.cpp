#include "GameEngine/Application.h"
#include "GameEngine/EntryPoint.h"

class ModelViewer : public Application
{
public:
	ModelViewer() {}
    ~ModelViewer() {}

    void InitializeApplication() override;
};

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new ModelViewer();
}

void ModelViewer::InitializeApplication()
{
	
}
