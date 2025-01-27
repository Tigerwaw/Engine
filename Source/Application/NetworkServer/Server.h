#pragma once
#include <string>
#include <WinSock2.h>
#include <thread>

#include "Communicator.h"

struct NetInfo
{
    std::string username;
    sockaddr_in address;

    bool operator==(const sockaddr_in& other) const
    {
        return address.sin_addr.S_un.S_addr == other.sin_addr.S_un.S_addr &&
            address.sin_port == other.sin_port;
    }

    bool operator==(sockaddr_in& other) const
    {
        return address.sin_addr.S_un.S_addr == other.sin_addr.S_un.S_addr &&
            address.sin_port == other.sin_port;
    }
};

class NetMessage;
class NetMessage_Connect;
class NetMessage_Disconnect;
class NetMessage_Text;

class Server
{
public:
    Server();
    ~Server();
protected:
    void Receive();
    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const;
    void HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived);

    void HandleMessage_Connect(NetMessage_Connect& aMessage, const sockaddr_in& aAddress);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage, const sockaddr_in& aAddress);
    void HandleMessage_Text(NetMessage_Text& aMessage, const sockaddr_in& aAddress, const int aBytesReceived);

    void HandleMessage_HandshakeRequest(const sockaddr_in& aAddress);

    void SendToAllClients(NetBuffer& aBuffer) const;
    bool DoesClientExist(const sockaddr_in& aAddress) const;
    const int GetClientIndex(const sockaddr_in& aAddress) const;

    Communicator myComm;
    std::vector<NetInfo> myClients;
    std::thread myReceiveThread;
};

