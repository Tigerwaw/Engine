#pragma once
#include "ClientBase.h"

class NetMessage_Connect;
class NetMessage_Disconnect;
class NetMessage_Text;

class GameObject;

class GameClient : public ClientBase
{
public:
    GameClient();
    void SendInput(std::string aMessage);

protected:
    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const override;
    void HandleMessage(NetMessage* aMessage) override;

    void SendHandshakeRequest() const override;

    void SendTextMessage(const std::string& aMessage) const;
    void SendConnectMessage(const std::string& aUsername) const;
    void SendDisconnectMessage() const;

    void HandleMessage_Connect(NetMessage_Connect& aMessage);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage);
    void HandleMessage_Text(NetMessage_Text& aMessage);

private:
    std::shared_ptr<GameObject> myPlayer;
    std::vector<std::shared_ptr<GameObject>> myRemotePlayers;
};