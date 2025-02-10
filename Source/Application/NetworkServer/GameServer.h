#pragma once
#include "ServerBase.h"

class NetMessage_Connect;
class NetMessage_Disconnect;
class NetMessage_Text;
class NetMessage_CreateCharacter;
class NetMessage_Position;

class GameServer : public ServerBase
{
public:
    void Update() override;
protected:
    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const override;
    void HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived) override;

    void HandleMessage_Connect(NetMessage_Connect& aMessage, const sockaddr_in& aAddress);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage, const sockaddr_in& aAddress);
    void HandleMessage_Text(NetMessage_Text& aMessage, const sockaddr_in& aAddress, const int aBytesReceived);
    void HandleMessage_Position(NetMessage_Position& aMessage, const sockaddr_in& aAddress);

    void HandleMessage_HandshakeRequest(const sockaddr_in& aAddress);

private:
    unsigned myCurrentNetworkID = 1;
};

