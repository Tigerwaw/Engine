#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class Movement2 : public Application
{
public:
	Movement2() {}
    ~Movement2() {}

    void InitializeApplication() override;
    void UpdateApplication() override;
};