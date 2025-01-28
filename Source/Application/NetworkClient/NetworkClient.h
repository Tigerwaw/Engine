#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

#include "ChatClient.h"

class NetworkClient : public Application
{
public:
	NetworkClient() {}
    ~NetworkClient() {}

    void InitializeApplication() override;
    void UpdateApplication() override;

private:
    ChatClient myClient;
};