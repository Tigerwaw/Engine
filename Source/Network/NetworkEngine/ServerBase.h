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

class ServerBase
{
public:
    ServerBase();
    virtual ~ServerBase();

    virtual void Update();
protected:
    void Receive();
    virtual NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const = 0;
    virtual void HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived) = 0;

    void SendToAllClients(NetBuffer& aBuffer) const;
    void SendToAllClientsExcluding(NetBuffer& aBuffer, const int aClientIndex) const;
    bool DoesClientExist(const sockaddr_in& aAddress) const;
    const int GetClientIndex(const sockaddr_in& aAddress) const;

    Communicator myComm;
    std::vector<NetInfo> myClients;

    std::thread myReceiveThread;
    bool myShouldReceive = true;
};

