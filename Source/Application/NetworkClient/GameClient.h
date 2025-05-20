#pragma once
#include "ClientBase.h"
#include "Utility/CircularArray.hpp"

class NetMessage_Connect;
class NetMessage_Disconnect;
class NetMessage_Text;
class NetMessage_CreateCharacter;
class NetMessage_RemoveCharacter;
class NetMessage_Position;
class NetMessage_Test;

class GameObject;

class GameClient : public ClientBase
{
public:
    void Update() override;

protected:
    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const override;
    void HandleMessage(NetMessage* aMessage) override;

    void SendHandshakeRequest() const override;

    void SendTextMessage(const std::string& aMessage) const;
    void SendConnectMessage(const std::string& aUsername) const;
    void SendDisconnectMessage() const;
    void SendPositionMessage(const CU::Vector3f& aPosition) const;

    void HandleMessage_Connect(NetMessage_Connect& aMessage);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage);
    void HandleMessage_Text(NetMessage_Text& aMessage);
    void HandleMessage_CreateCharacter(NetMessage_CreateCharacter& aMessage);
    void HandleMessage_RemoveCharacter(NetMessage_RemoveCharacter& aMessage);
    void HandleMessage_Position(NetMessage_Position& aMessage);
    void HandleMessage_Test(NetMessage_Test& aMessage);

private:
    std::unordered_map<unsigned, CircularArray<CU::Vector3f, 16>> myObjectIDPositionHistory;
};