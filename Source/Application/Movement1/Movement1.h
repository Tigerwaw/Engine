#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class Movement1 : public Application
{
public:
	Movement1() {}
    ~Movement1() {}

    void InitializeApplication() override;
    void UpdateApplication() override;
};