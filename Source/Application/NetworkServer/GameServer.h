#pragma once
#include <string>
#include <WinSock2.h>
#include <thread>
#include <chrono>
#include "Utilities/Math/Vector.hpp"

#include "Communicator.h"

struct GuaranteedMessageData
{
    int myAttempts = 0;
    std::chrono::system_clock::time_point myLastSentTimestamp;
    NetBuffer myGuaranteedMessageBuffer;
    sockaddr_in myRecipientAddress;
};

struct AckMessageData
{
    int myMessageID;
    sockaddr_in mySenderAddress;

    bool operator==(const AckMessageData& other) const
    {
        return myMessageID == other.myMessageID && 
            mySenderAddress.sin_addr.S_un.S_addr == other.mySenderAddress.sin_addr.S_un.S_addr &&
            mySenderAddress.sin_port == other.mySenderAddress.sin_port;
    }
};

struct NetInfo
{
    std::string myUsername;
    sockaddr_in myAddress;
    unsigned myCharacterNetworkID;
    int myLastPingMessageID;
    std::chrono::system_clock::time_point myLastPingTime;
    float myRTT;
    std::chrono::system_clock::time_point myLastNearUpdateTimestamp;
    std::chrono::system_clock::time_point myLastMediumUpdateTimestamp;
    std::chrono::system_clock::time_point myLastFarUpdateTimestamp;

    bool operator==(const sockaddr_in& other) const
    {
        return myAddress.sin_addr.S_un.S_addr == other.sin_addr.S_un.S_addr &&
            myAddress.sin_port == other.sin_port;
    }

    bool operator==(const NetInfo& other) const
    {
        return myAddress.sin_addr.S_un.S_addr == other.myAddress.sin_addr.S_un.S_addr &&
            myAddress.sin_port == other.myAddress.sin_port;
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
    const std::unordered_map<int, GuaranteedMessageData>& GetGuaranteedMessageData() const { return myGuaranteedMessageIDToData; }
protected:
    void Receive();
    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const;
    void HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress);
    bool HandleGuaranteedMessage(GuaranteedNetMessage* aMessage, const sockaddr_in& aAddress);
    void HandleAckMessage(AckNetMessage* aMessage, const sockaddr_in& aAddress);

    void AcceptHandshake(const NetBuffer& aBuffer, const sockaddr_in& aAddress);
    NetInfo& AddClient(const sockaddr_in& aAddress, const std::string& aUsername);
    void RemoveClient(const NetInfo& aClientNetInfo);

    void SendToClient(const NetBuffer& aBuffer, const NetInfo& aClientNetInfo);
    void SendToAllClients(const NetBuffer& aBuffer);
    void SendToAllClientsExcluding(const NetBuffer& aBuffer, const NetInfo& aClientNetInfo);

    bool DoesClientExist(const sockaddr_in& aAddress) const;

    const NetInfo* GetClient(const sockaddr_in& aAddress) const;
    NetInfo* GetClient(const sockaddr_in& aAddress);
    const NetInfo* GetClientByNetworkID(unsigned aNetworkID) const;
    NetInfo* GetClientByNetworkID(unsigned aNetworkID);

    void UpdateClientPing(NetInfo& aClientNetInfo, int aMessageID, bool aShouldUpdateRTT = false);

    void HandleMessage_RequestConnect(NetMessage_RequestConnect& aMessage, const sockaddr_in& aAddress);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage, const sockaddr_in& aAddress);

    void HandleMessage_HandshakeRequest(const sockaddr_in& aAddress);
    void HandleMessage_Position(NetMessage_Position& aMessage, const sockaddr_in& aAddress);

    void CreateNewObject();
    void DestroyObject(unsigned aNetworkID);

    void CreateNewPlayer(NetInfo& aClientNetInfo);

    void UpdateNetworkStats();
    void PingClients();
    void UpdateGuaranteedMessages();
    void UpdateObjectSpawn();
    void UpdatePositions();

    void SendTestMessage();

    void InitializeGrid();
    Math::Vector2<int> GetGridCell(const Math::Vector3f& aPosition) const;

private:
    Communicator myComm;
    std::vector<NetInfo> myClients;
    bool myShouldReceive = false;
    std::chrono::system_clock::time_point myLastTickTimestamp;
    unsigned myCurrentNetworkID = 1;

    // Game Specific
    int myCurrentlyActiveObjects = 0;
    std::chrono::system_clock::time_point myLastSpawnTimestamp;
    std::vector<std::shared_ptr<GameObject>> myObjects;
    std::vector<std::shared_ptr<GameObject>> myPlayers;
    std::array<std::array<Math::AABB3D<float>, 2>, 2> myGrid;
    // --

    // Network Stats
    int myDataReceived = 0;
    int myDataSent = 0;
    int myAvgDataReceived = 0;
    int myAvgDataSent = 0;
    std::chrono::system_clock::time_point myLastDataTickTime;
    int myNrOfGuaranteedMessagesSent = 0;
    int myNrOfAcknowledges = 0;
    // --

    // Guaranteed Message Handling
    int myCurrentGuaranteedMessageID = 1;
    std::unordered_map<int, GuaranteedMessageData> myGuaranteedMessageIDToData;
    std::vector<AckMessageData> myAcknowledgedMessageIDs;
    [[nodiscard("You need to use the returned message ID to add the buffer to myGuaranteedMessageIDToData")]] int CreateNewGuaranteedMessage(GuaranteedNetMessage* aGuaranteedMessage, const NetInfo& aClientNetInfo);
    // --
};

