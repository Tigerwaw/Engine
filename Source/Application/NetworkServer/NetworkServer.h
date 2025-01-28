#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include "ChatServer.h"

class NetworkServer : public Application
{
public:
	NetworkServer() {}
    ~NetworkServer() {}

    void InitializeApplication() override;
    void UpdateApplication() override;

private:
    ChatServer myServer;
};