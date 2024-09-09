#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class PathfindingTest : public Application
{
public:
	PathfindingTest() {}
    ~PathfindingTest() {}

    void InitializeApplication() override;
    void UpdateApplication() override;
};
