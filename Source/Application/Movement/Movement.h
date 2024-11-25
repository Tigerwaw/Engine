#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class Movement : public Application
{
public:
    Movement() {}
    ~Movement() {}

    void InitializeApplication() override;
    void UpdateApplication() override;

private:
    std::vector<std::shared_ptr<GameObject>> myActors;
};