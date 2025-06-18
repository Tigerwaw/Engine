#pragma once
#include <string>
#include <WinSock2.h>
#include <thread>
#include <chrono>

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
    virtual ~ServerBase();
    void StartServer();

    virtual void Update();

    int GetReceivedData() const { return myAvgDataReceived; }
    int GetSentData() const { return myAvgDataSent; }
protected:
    void Receive();
    virtual NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const = 0;
    virtual void HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived) = 0;

    void AcceptHandshake(const NetBuffer& aBuffer, const sockaddr_in& aAddress);
    const NetInfo& AddClient(const sockaddr_in& aAddress, const std::string& aUsername);
    void RemoveClient(int aClientIndex); 

    void SendToClient(const NetBuffer& aBuffer, int aClientIndex);
    void SendToAllClients(const NetBuffer& aBuffer);
    void SendToAllClientsExcluding(const NetBuffer& aBuffer, const int aClientIndex);
    bool DoesClientExist(const sockaddr_in& aAddress) const;
    const int GetClientIndex(const sockaddr_in& aAddress) const;
    const NetInfo& GetClient(int aClientIndex) const;

    bool myShouldReceive = false;
    int myMessagesHandledPerTick = 10;

private:
    Communicator myComm;
    std::vector<NetInfo> myClients;

    int myDataReceived = 0;
    int myDataSent = 0;
    int myAvgDataReceived = 0;
    int myAvgDataSent = 0;
    std::chrono::system_clock::time_point myLastDataTickTime;
    float myDataTickRate = 1.0f;
};

