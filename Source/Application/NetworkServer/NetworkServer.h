#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include "GameServer.h"

class NetworkServer : public Application
{
public:
	NetworkServer() {}
    ~NetworkServer() {}

    void InitializeApplication() override;
    void UpdateApplication() override;

private:
    GameServer myServer;
};