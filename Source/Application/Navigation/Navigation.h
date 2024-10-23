#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include "GameEngine/DebugDrawer/DebugLine.hpp"

class NavMesh;

class Navigation : public Application
{
public:
	Navigation() {}
    ~Navigation() {}

    void InitializeApplication() override;
    void UpdateApplication() override;

private:
    void CastRay();

    std::shared_ptr<NavMesh> myNavMesh;
    DebugLine myDebugRay;
};