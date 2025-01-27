#pragma once
#include <string>
#include <thread>

#include "Communicator.h"

class NetMessage;
class NetMessage_Connect;
class NetMessage_Disconnect;
class NetMessage_Text;

class Client
{
public:
    Client();
    ~Client();

    void SendInput(std::string aMessage);

protected:
    void Receive();
    void SendHandshakeRequest() const;
    void SendTextMessage(const std::string& aMessage) const;
    void SendConnectMessage(const std::string& aUsername) const;
    void SendDisconnectMessage() const;

    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const;
    void HandleMessage(NetMessage* aMessage);

    void HandleMessage_Connect(NetMessage_Connect& aMessage);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage);
    void HandleMessage_Text(NetMessage_Text& aMessage);

    void HandleMessage_HandshakeAccept();

    Communicator myComm;
    bool myHasEstablishedHandshake = false;
    bool myHasEstablishedConnection = false;

    std::thread myReceiveThread;
};