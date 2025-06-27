#pragma once
#include <string>
#include <thread>
#include <chrono>
#include "Math/Vector.hpp"
#include "CommonUtilities/CircularArray.hpp"
#include "Communicator.h"

class NetMessage;
class GuaranteedNetMessage;
class AckNetMessage;
class NetMessage_Connect;
class NetMessage_AcceptConnect;
class NetMessage_Disconnect;
class NetMessage_Text;
class NetMessage_CreateCharacter;
class NetMessage_RemoveCharacter;
class NetMessage_Position;
class NetMessage_Test;

class GameObject;

struct GuaranteedMessageData
{
    int myAttempts = 0;
    std::chrono::system_clock::time_point myLastSentTimestamp;
    NetBuffer myGuaranteedMessageBuffer;
};

struct PositionData
{
    Math::Vector3f position;
    std::chrono::system_clock::time_point clientTimestamp;
    std::chrono::system_clock::time_point serverTimestamp;
};

class GameClient
{
public:
    void SetUsername(const char* aUsername);
    void ConnectClient(const char* aIP);
    void Update();

    void ToggleLerpPositions(bool aToggle) { myShouldLerpPositions = aToggle; }
    bool GetIsLerpingPositions() const { return myShouldLerpPositions; }

    int GetReceivedData() const { return myAvgDataReceived; }
    int GetSentData() const { return myAvgDataSent; }
    int GetNrOfGuaranteedMessagesSent() const { return myNrOfGuaranteedMessagesSent; }
    int GetNrOfGuaranteedMessagesLost() const { return myNrOfGuaranteedMessagesSent - myNrOfAcknowledges; }
    float GetEstimatedPackageLoss() const { return static_cast<float>(myNrOfGuaranteedMessagesSent - myNrOfAcknowledges) / static_cast<float>(myNrOfGuaranteedMessagesSent); }
    float GetRTT() const { return myRTT; }
    const std::unordered_map<int, GuaranteedMessageData>& GetGuaranteedMessageData() const { return myGuaranteedMessageIDToData; }
    const std::unordered_map<unsigned, Utilities::CircularArray<PositionData, 16>>& GetObjectPositionData() const { return myObjectIDPositionHistory; }
protected:
    void Receive();
    void Send(const NetBuffer& aBuffer);

    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const;
    void HandleMessage(NetMessage* aMessage);
    bool HandleGuaranteedMessage(GuaranteedNetMessage* aMessage);
    void HandleAckMessage(AckNetMessage* aMessage);

    void SendHandshakeRequest();
    void SendConnectionRequest(const std::string& aUsername);
    void SendDisconnectMessage();
    void SendTextMessage(const std::string& aMessage);
    void SendPositionMessage(const Math::Vector3f& aPosition, unsigned aNetworkID);

    void HandleMessage_AcceptHandshake();
    void HandleMessage_AcceptConnect();
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage);
    void HandleMessage_Text(NetMessage_Text& aMessage);
    void HandleMessage_CreateCharacter(NetMessage_CreateCharacter& aMessage);
    void HandleMessage_RemoveCharacter(NetMessage_RemoveCharacter& aMessage);
    void HandleMessage_Position(NetMessage_Position& aMessage);
    void HandleMessage_Test(NetMessage_Test& aMessage);

    bool HasEstablishedHandshake() const { return myHasEstablishedHandshake; }
    bool HasEstablishedConnection() const { return myHasEstablishedConnection; }

    void TryConnect();
    void UpdateNetworkStats();
    void PingServer();
    void UpdateGuaranteedMessages();
    void UpdateObjectPositions();
    void SendPlayerCharacterPosition();
    void Disconnect();

private:
    Communicator myComm;
    std::chrono::system_clock::time_point myLastTickTimestamp;
    std::chrono::system_clock::time_point myLastHandshakeRequestTime;
    std::chrono::system_clock::time_point myLastConnectRequestTime;
    bool myHasEstablishedHandshake = false;
    bool myHasEstablishedConnection = false;
    bool myShouldReceive = false;
    bool myAttemptToConnect = false;
    std::string myUsername;

    // Network Stats
    int myDataReceived = 0;
    int myDataSent = 0;
    int myAvgDataReceived = 0;
    int myAvgDataSent = 0;
    std::chrono::system_clock::time_point myLastDataTickTime;
    int myNrOfGuaranteedMessagesSent = 0;
    int myNrOfAcknowledges = 0;
    // --

    // Server Ping
    std::chrono::system_clock::time_point myLastPingTime;
    float myRTT;
    // --

    // Object Position Lerping
    std::unordered_map<unsigned, Utilities::CircularArray<PositionData, 16>> myObjectIDPositionHistory;
    bool myShouldLerpPositions = true;
    // --

    // Guaranteed Message Handling
    int myCurrentGuaranteedMessageID = 0;
    std::unordered_map<int, GuaranteedMessageData> myGuaranteedMessageIDToData;
    std::vector<int> myAcknowledgedMessageIDs;
    [[nodiscard("You need to use the returned message ID to add the buffer to myGuaranteedMessageIDToData")]] int CreateNewGuaranteedMessage(GuaranteedNetMessage* aGuaranteedMessage);
    // --

    // Player Character
    std::shared_ptr<GameObject> myPlayerCharacter;
    std::chrono::system_clock::time_point myLastPositionSendTimestamp;
    Math::Vector3f myLastPosition;
    // --
};