#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class DecisionMaking : public Application
{
public:
	DecisionMaking() {}
    ~DecisionMaking() {}

    void InitializeApplication() override;
    void UpdateApplication() override;
};