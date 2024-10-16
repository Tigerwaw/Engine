#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class WorldInterfacing : public Application
{
public:
	WorldInterfacing() {}
    ~WorldInterfacing() {}

    void InitializeApplication() override;
};