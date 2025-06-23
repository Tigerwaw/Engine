#pragma once
#include <string>
#include <WinSock2.h>
#include <thread>
#include <chrono>

#include "Communicator.h"

struct GuaranteedMessageData
{
    int myAttempts = 0;
    std::chrono::system_clock::time_point myLastSentTimestamp;
    NetBuffer myGuaranteedMessageBuffer;
    sockaddr_in myRecipientAddress;
};

struct NetInfo
{
    std::string username;
    sockaddr_in address;
    int myLastPingMessageID;
    std::chrono::system_clock::time_point myLastPingTime;
    float myRTT;

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
class GuaranteedNetMessage;
class AckNetMessage;
class NetMessage_RequestConnect;
class NetMessage_Disconnect;
class NetMessage_Text;
class NetMessage_CreateCharacter;
class NetMessage_Position;

class GameServer
{
public:
    void StartServer();
    void Update();

    int GetReceivedData() const { return myAvgDataReceived; }
    int GetSentData() const { return myAvgDataSent; }
    int GetNrOfGuaranteedMessagesSent() const { return myNrOfGuaranteedMessagesSent; }
    int GetNrOfGuaranteedMessagesLost() const { return myNrOfGuaranteedMessagesSent - myNrOfAcknowledges; }
    float GetEstimatedPackageLoss() const { return static_cast<float>(myNrOfGuaranteedMessagesSent - myNrOfAcknowledges) / static_cast<float>(myNrOfGuaranteedMessagesSent); }
    const std::vector<NetInfo>& GetClients() const { return myClients; }
protected:
    void Receive();
    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const;
    void HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived);

    void AcceptHandshake(const NetBuffer& aBuffer, const sockaddr_in& aAddress);
    const NetInfo& AddClient(const sockaddr_in& aAddress, const std::string& aUsername);
    void RemoveClient(int aClientIndex);

    void SendToClient(const NetBuffer& aBuffer, int aClientIndex);
    void SendToAllClients(const NetBuffer& aBuffer);
    void SendToAllClientsExcluding(const NetBuffer& aBuffer, const int aClientIndex);
    bool DoesClientExist(const sockaddr_in& aAddress) const;
    const int GetClientIndex(const sockaddr_in& aAddress) const;
    const NetInfo& GetClient(int aClientIndex) const;
    void UpdateClientPing(int aClientIndex, int aMessageID, bool aShouldUpdateRTT = false);

    bool myShouldReceive = false;
    int myMessagesHandledPerTick = 10;

    void HandleMessage_RequestConnect(NetMessage_RequestConnect& aMessage, const sockaddr_in& aAddress);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage, const sockaddr_in& aAddress);

    void HandleMessage_HandshakeRequest(const sockaddr_in& aAddress);

    void CreateNewObject();
    void DestroyObject(unsigned aNetworkID);
    void UpdatePositions();

    void SendTestMessage();

private:
    Communicator myComm;
    std::vector<NetInfo> myClients;

    int myDataReceived = 0;
    int myDataSent = 0;
    int myAvgDataReceived = 0;
    int myAvgDataSent = 0;
    std::chrono::system_clock::time_point myLastDataTickTime;
    float myDataTickRate = 1.0f;
    int myNrOfGuaranteedMessagesSent = 0;
    int myNrOfAcknowledges = 0;

    unsigned myCurrentNetworkID = 1;
    std::vector<std::shared_ptr<GameObject>> myObjects;

    float myTickRate = 10.0f;

    int myCurrentlyActiveObjects = 0;
    int myObjectLimit = 16;

    double myLastUpdateTimestamp = 0;
    float myTimeBetweenObjectsSpawned = 1.0f;
    float myCurrentTimeSinceLastSpawn = 0;

    int myGuaranteedMessageID = 1;
    std::unordered_map<int, GuaranteedMessageData> myGuaranteedMessageIDToData;
    float myGuaranteedMessageTimeout = 1.0f;
    int myGuaranteedMesssageMaxTimeouts = 3;
    std::vector<int> myAcknowledgedMessageIDs;
    [[nodiscard("You need to use the returned message ID to add the buffer to myGuaranteedMessageIDToData")]] int CreateNewGuaranteedMessage(GuaranteedNetMessage* aGuaranteedMessage, int aClientIndex);

    float myTimeBetweenPings = 1.0f;
};

